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
    peak_heap_extra_B = 0
    peak_stacks_B = 0

    for line in fl:
        if line[:11] == "mem_heap_B=":
            peak_heap_B = max(peak_heap_B, int(line[11:]))
        elif line[:17] == "mem_heap_extra_B=":
            peak_heap_extra_B = max(peak_heap_extra_B, int(line[17:]))
        elif line[:13] == "mem_stacks_B=":
            peak_stacks_B = max(peak_stacks_B, int(line[13:]))

    return {"peak_heap": peak_heap_B, "peak_heap_extra": peak_heap_extra_B, "peak_stacks": peak_stacks_B}


class Run:
    def __init__(self, v=0, e=0, sv=1, se=100, t=1, a=''):
        self.v = v  # Maximum number of vertices
        self.e = e  # Maximum number of edges
        self.sv = sv  # Vertices' stride
        self.se = se  # Edges' stride
        self.t = t  # Number of trials for (edge, vertex) configuration
        self.a = a  # Algorithm


runs = [Run(v=50, e=250, sv=1, se=2, t=10, a='p')]

runs1 = [Run(v=500, e=2500, sv=5, se=10, t=10, a='t'),
         Run(v=500, e=2500, sv=5, se=10, t=10, a='n'),
         Run(v=500, e=2500, sv=5, se=10, t=10, a='p'),
         Run(v=2000, e=15000, sv=20, se=100, t=10, a='t'),
         Run(v=2000, e=15000, sv=20, se=100, t=10, a='n'),
         Run(v=2000, e=15000, sv=20, se=100, t=10, a='p')]

graphPath = "./graphs/graph.xml"
fullMainPath = "./space_complexity_graphs/fullMain"
emptyMainPath = "./space_complexity_graphs/emptyMain"


for run in runs:
    uV = int(math.floor(run.v / run.sv))  # Unitary V
    uE = int(math.floor(run.e / run.se))  # Unitary E

    # Declaring the uV x uE matrix storing the time measurements
    time_matrix = [0] * uV
    for i in range(uV):
        time_matrix[i] = [0] * uE

    # Declaring the uV x uE matrix storing the space measurements
    space_matrix = [0] * uV
    for i in range(uV):
        space_matrix[i] = [0] * uE


    # Declaring the uV x uE matrix storing the heap space measurements on empty main()
    emptyMainHeap = [0] * uV
    for i in range(uV):
        emptyMainHeap[i] = [0] * uE

    # Declaring the uV x uE matrix storing the stack space measurements on empty main()
    emptyMainStack = [0] * uV
    for i in range(uV):
        emptyMainStack[i] = [0] * uE

    # Declaring the uV x uE matrix storing the heap space measurements on main with running algorithms
    fullMainHeap = [0] * uV
    for i in range(uV):
        fullMainHeap[i] = [0] * uE

    # Declaring the uV x uE matrix storing the stack space measurements on main with running algorithms
    fullMainStack = [0] * uV
    for i in range(uV):
        fullMainStack[i] = [0] * uE

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
                check_call(["./cmake-build-debug/graphGenerator" ,
                            str((v + 1) * run.sv) , str((e + 1) * run.se) , graphPath])

                # time measurement
                outputJSON = check_output(
                    ["./cmake-build-debug/AAProject", str((v + 1) * run.sv), str((e + 1) * run.se), graphPath, run.a])
                output = json.loads(str(outputJSON))
                elapsedTime = int(output['elapsed_time'])
                stackDimension = int(output['stack_dimension'])

                # full run of the algorithm, space complexity measured with Massif
                check_call(["valgrind --tool=massif --massif-out-file=" + fullMainPath + " --stacks=yes "
                            "./cmake-build-debug/AAProject", str((v + 1) * run.sv), str((e + 1) * run.se), graphPath, run.a])

                space_dict = get_peaks(fullMainPath)
                fullMainHeap[v][e] += space_dict.get("peak_heap")
                fullMainStack[v][e] += space_dict.get("peak_stacks")

                # run of main() only, space complexity measured with Massif
                check_call(["valgrind --tool=massif --massif-out-file=" + emptyMainPath + " --stacks=yes "
                            "./cmake-build-debug/AAProject", str((v + 1) * run.sv), str((e + 1) * run.se), graphPath, "s"])
                space_dict = get_peaks(emptyMainPath)

                emptyMainHeap[v][e] += space_dict.get("peak_heap")
                emptyMainStack[v][e] += space_dict.get("peak_stacks")



                # updating matrices for least squares
                phi[(uE*v)+e, 0] = (v + 1) * run.sv
                phi[(uE*v)+e, 1] = (e + 1) * run.se
                y[(uE * v) + e] += elapsedTime

                # Updating time matrix
                time_matrix[v][e] += elapsedTime

                # Updating space matrix
                space_matrix[v][e] += stackDimension

                bar.update((v + 1) * (e + 1) * (t + 1))
    bar.finish()

    heap_matrix = [0] * uV
    for i in range(uV):
        heap_matrix[i] = [0] * uE

    stack_matrix = [0] * uV
    for i in range(uV):
        stack_matrix[i] = [0] * uE

    # Divide the whole element of the matrices by run.t (so to get the average time)
    for v in range(0, uV):
        for e in range(0, uE):
            time_matrix[v][e] = int(math.ceil(time_matrix[v][e] / run.t))
            space_matrix[v][e] = int(math.ceil(space_matrix[v][e] / run.t))

            # obtain the algorithm's space complexity as the difference between a full execution and a run of main() only
            stack_matrix[v][e] = int(math.ceil(fullMainStack[v][e] / run.t)) - int(math.ceil(emptyMainStack[v][e] / run.t))
            heap_matrix[v][e] = int(math.ceil(fullMainHeap[v][e] / run.t)) - int(math.ceil(emptyMainHeap[v][e] / run.t))


    ls_result = ((np.matrix(phi).T * np.matrix(phi)).I * np.matrix(phi).T) * y
    ls = "complexity: " + str(int(ls_result[0]/run.t)) + "*V + " + str(int(ls_result[1]/run.t)) + "*E"

    time_data = [go.Surface(z=np.matrix(time_matrix).T)]
    space_data = [go.Surface(z=np.matrix(space_matrix).T)]
    stack_data = [go.Surface(z=np.matrix(stack_matrix).T)]
    heap_data = [go.Surface(z=np.matrix(heap_matrix).T)]

    graph_info = "a: " + run.a + ", V: " + str(run.v) + ", E: " + str(run.e) + \
                 ", SV: " + str(run.sv) + ", SE: " + str(run.se) + ", T: " + str(run.t)
    time_title = "Elapsed time [ms] <" + graph_info + "> -> " + ls
    space_title = "Stack dimension [byte] <" + graph_info + "> -> " + ls
    heap_title = "Max heap size [byte] <" + graph_info
    stack_title = "Max stack size [byte] <" + graph_info


    time_layout = go.Layout(title=time_title, autosize=True)
    space_layout = go.Layout(title=space_title, autosize=True)
    heap_layout = go.Layout(title=heap_title, autosize=True)
    stack_layout = go.Layout(title=stack_title, autosize=True)

    time_fig = go.Figure(data=time_data, layout=time_layout)
    space_fig = go.Figure(data=space_data, layout=space_layout)
    heap_fig = go.Figure(data=heap_data, layout=heap_layout)
    stack_fig = go.Figure(data=stack_data, layout=stack_layout)

    py.offline.plot(time_fig,
                    filename="time_complexity_graphs/time_complexity__A" + run.a + "V" + str(run.v) + "E" + str(
                        run.e) + ", SV: " + str(run.sv) + ", SE: " + str(run.se) + "T" + str(run.t) + ".html")
    py.offline.plot(space_fig,
                    filename="space_complexity_graphs/space_complexity__A" + run.a + "V" + str(run.v) + "E" + str(
        run.e) + ", SV: " + str(run.sv) + ", SE: " + str(run.se) + "T" + str(run.t) + ".html")

    py.offline.plot(heap_fig,
                    filename="space_complexity_graphs/heap_complexity__A" + run.a + "V" + str(run.v) + "E" + str(
                        run.e) + ", SV: " + str(run.sv) + ", SE: " + str(run.se) + "T" + str(run.t) + ".html")

    py.offline.plot(space_fig,
                    filename="space_complexity_graphs/stack_complexity__A" + run.a + "V" + str(run.v) + "E" + str(
                        run.e) + ", SV: " + str(run.sv) + ", SE: " + str(run.se) + "T" + str(run.t) + ".html")



