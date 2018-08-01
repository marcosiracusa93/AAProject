import json
import math
from subprocess import check_output, check_call

import numpy as np
import plotly as py
import plotly.graph_objs as go
import progressbar


def get_peaks(filename):
    f = open(filename, "r")

    fl = f.readlines()

    peak_heap_B = 0
    peak_stacks_B = 0

    for line in fl:
        if line[:11] == "mem_heap_B=":
            peak_heap_B = max(peak_heap_B, int(line[11:]))
        elif line[:13] == "mem_stacks_B=":
            peak_stacks_B = max(peak_stacks_B, int(line[13:]))
    f.close()

    return {"peak_heap": peak_heap_B, "peak_stack": peak_stacks_B}


class Run:
    def __init__(self, v=0, e=0, sv=1, se=100, t=1, a=''):
        self.v = v  # Maximum number of vertices
        self.e = e  # Maximum number of edges
        self.sv = sv  # Vertices' stride
        self.se = se  # Edges' stride
        self.t = t  # Number of trials for (edge, vertex) configuration
        self.a = a  # Algorithm


runs = [Run(v=400, e=10000, sv=50, se=1000, t=1, a='p'),
        Run(v=400, e=10000, sv=50, se=1000, t=1, a='n'),
        Run(v=400, e=10000, sv=50, se=1000, t=1, a='t')]

graphPath = "./graphs/graph.xml"
tarjPath = "./graphs/tarj"
nuutPath = "./graphs/nuut"
peaPath = "./graphs/pea"
graphOnlyPath ="./graphs/graphOnly"


for run in runs:
    uV = int(math.floor(run.v / run.sv))  # Unitary V
    uE = int(math.floor(run.e / run.se))  # Unitary E

    # Declaring the uV x uE matrix storing the time measurements
    time_matrix = [0] * uV
    for i in range(uV):
        time_matrix[i] = [0] * uE

    # Declaring the uV x uE matrix storing the heap space measurements on main with running algorithms
    diffHeap = [0] * uV
    for i in range(uV):
        diffHeap[i] = [0] * uE

    heap_matrix = [0] * uV
    for i in range(uV):
        heap_matrix[i] = [0] * uE

    graph_matrix_heap = [0] * uV
    for i in range(uV):
        graph_matrix_heap[i] = [0] * uE


    # Declaring two matrices for least squares
    phi = np.zeros((uV * uE, 2))
    y = np.zeros((uV * uE, 1))

    bar = progressbar.ProgressBar(maxval=uV * uE * run.t,
                                  widgets=[progressbar.Bar('=', '[', ']'), ' ', progressbar.Percentage()])

    bar.start()
    # Sum the run.t time measurements taken from each (vertex,edge) configuration
    for t in range(run.t):
        for v in range(0, uV):
            for e in range(0, uE):
                check_call(['valgrind', '--tool=massif', '--massif-out-file=' + graphOnlyPath,
                            './cmake-build-debug/graphGenerator',
                            str((v+1) * run.sv), str((e+1) * run.se), graphPath])

                # graph only space measurement
                space_dict = get_peaks(graphOnlyPath)
                graph_matrix_heap[v][e] += space_dict.get("peak_heap")

                if run.a == "t":
                    outputJSON = check_output(
                        ["./cmake-build-debug/tarj_find_scc1_main", str((v + 1) * run.sv), str((e + 1) * run.se), graphPath])
                    # full run of the algorithm, space complexity measured with Massif
                    check_output(['valgrind', '--tool=massif', '--massif-out-file=' + tarjPath,
                                  './cmake-build-debug/tarj_find_scc1_main', str((v + 1) * run.sv), str((e + 1) * run.se), graphPath])
                    space_dict = get_peaks(tarjPath)

                elif run.a == "n":
                    outputJSON = check_output(
                        ["./cmake-build-debug/nuut_find_scc2_main", str((v + 1) * run.sv), str((e + 1) * run.se), graphPath])
                    # full run of the algorithm, space complexity measured with Massif
                    check_output(['valgrind', '--tool=massif', '--massif-out-file=' + nuutPath,
                                  './cmake-build-debug/nuut_find_scc2_main', str((v + 1) * run.sv), str((e + 1) * run.se), graphPath])
                    space_dict = get_peaks(nuutPath)

                elif run.a == "p":
                    outputJSON = check_output(
                        ["./cmake-build-debug/pea_find_scc3_main", str((v + 1) * run.sv), str((e + 1) * run.se), graphPath])
                    # full run of the algorithm, space complexity measured with Massif
                    check_output(['valgrind', '--tool=massif', '--massif-out-file=' + peaPath,
                                  './cmake-build-debug/pea_find_scc3_main', str((v + 1) * run.sv), str((e + 1) * run.se), graphPath])
                    space_dict = get_peaks(peaPath)


                output = json.loads(outputJSON)
                elapsedTime = int(output['elapsed_time'])
                heap_matrix[v][e] += space_dict.get("peak_heap")

                # updating matrices for least squares
                phi[(uE*v)+e, 0] = (v + 1) * run.sv
                phi[(uE*v)+e, 1] = (e + 1) * run.se
                y[(uE * v) + e] += elapsedTime

                # Updating time matrix
                time_matrix[v][e] += elapsedTime

                bar.update((v + 1) * (e + 1) * (t + 1))
    bar.finish()


    # Divide the whole element of the matrices by run.t (so to get the average time)
    for v in range(0, uV):
        for e in range(0, uE):
            time_matrix[v][e] = int(math.ceil(time_matrix[v][e] / run.t))

            heap_matrix[v][e] = int(math.ceil(heap_matrix[v][e] / run.t))

            graph_matrix_heap[v][e] = int(math.ceil(graph_matrix_heap[v][e] / run.t))

            diffHeap[v][e] = heap_matrix[v][e] - graph_matrix_heap[v][e]

    ls_result = ((np.matrix(phi).T * np.matrix(phi)).I * np.matrix(phi).T) * y
    ls = "complexity: " + str(ls_result[0]/run.t) + "*V + " + str(ls_result[1]/run.t) + "*E"

    time_data = [go.Surface(z=np.matrix(time_matrix).T)]
    heap_data = [go.Surface(z=np.matrix(heap_matrix).T)]
    diff_heap_data = [go.Surface(z=np.matrix(diffHeap).T)]
    g_heap_data = [go.Surface(z=np.matrix(graph_matrix_heap).T)]

    graph_info = "a: " + run.a + ", V: " + str(run.v) + ", E: " + str(run.e) + \
                 ", SV: " + str(run.sv) + ", SE: " + str(run.se) + ", T: " + str(run.t)

    time_title = "Elapsed time [ms] <" + graph_info + " -> " + ls
    heap_title = "Max heap size [byte] <" + graph_info
    diff_title = "Heap difference <" + graph_info
    g_heap_title = "Max heap size (GRAPH ONLY) [byte] <" + graph_info

    time_layout = go.Layout(title=time_title, autosize=True)
    heap_layout = go.Layout(title=heap_title, autosize=True)
    diff_layout = go.Layout(title=diff_title, autosize=True)
    g_heap_layout = go.Layout(title=g_heap_title, autosize=True)

    time_fig = go.Figure(data=time_data, layout=time_layout)
    heap_fig = go.Figure(data=heap_data, layout=heap_layout)
    diff_fig = go.Figure(data=diff_heap_data, layout=diff_layout)
    g_heap_fig = go.Figure(data=g_heap_data, layout=g_heap_layout)

    py.offline.plot(time_fig,
                    filename="time_complexity_graphs/time_complexity__A" + run.a + "V" + str(run.v) + "E" + str(
                        run.e) + ", SV: " + str(run.sv) + ", SE: " + str(run.se) + "T" + str(run.t) + ".html")

    py.offline.plot(heap_fig,
                    filename="space_complexity_graphs/heap_complexity__A" + run.a + "V" + str(run.v) + "E" + str(
                        run.e) + ", SV: " + str(run.sv) + ", SE: " + str(run.se) + "T" + str(run.t) + ".html")

    py.offline.plot(diff_fig,
                    filename="space_complexity_graphs/diff_heap_complexity__A" + run.a + "V" + str(run.v) + "E" + str(
                        run.e) + ", SV: " + str(run.sv) + ", SE: " + str(run.se) + "T" + str(run.t) + ".html")

    py.offline.plot(g_heap_fig,
                    filename="space_complexity_graphs/heap_complexity(GRAPH ONLY)__A" + run.a + "V" + str(run.v) + "E" + str(
                        run.e) + ", SV: " + str(run.sv) + ", SE: " + str(run.se) + "T" + str(run.t) + ".html")





