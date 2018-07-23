import json
import math
from subprocess import check_output

import numpy as np
import plotly as py
import plotly.graph_objs as go
import progressbar


class Run:
    def __init__(self, v=0, e=0, sv=1, se=100, t=1, a=''):
        self.v = v  # Maximum number of vertices
        self.e = e  # Maximum number of edges
        self.sv = sv  # Vertices' stride
        self.se = se  # Edges' stride
        self.t = t  # Number of trials for (edge, vertex) configuration
        self.a = a  # Algorithm


runs = [Run(v=500, e=2500, sv=50, se=250, t=10, a='p'),
        Run(v=2000, e=15000, sv=200, se=1500, t=10, a='p')]

runs1 = [Run(v=500, e=2500, sv=5, se=10, t=10, a='t'),
         Run(v=500, e=2500, sv=5, se=10, t=10, a='n'),
         Run(v=500, e=2500, sv=5, se=10, t=10, a='p'),
         Run(v=2000, e=15000, sv=20, se=100, t=10, a='t'),
         Run(v=2000, e=15000, sv=20, se=100, t=10, a='n'),
         Run(v=2000, e=15000, sv=20, se=100, t=10, a='p')]

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
                outputJSON = check_output(
                    ["./cmake-build-debug/AAProject", str((v + 1) * run.sv), str((e + 1) * run.se), run.a])
                output = json.loads(outputJSON)
                elapsedTime = int(output['elapsed_time'])
                stackDimension = int(output['stack_dimension'])

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

    # Divide the whole element of the matrix by run.t (so to get the average time)
    for v in range(0, uV):
        for e in range(0, uE):
            time_matrix[v][e] = int(math.ceil(time_matrix[v][e] / run.t))
            space_matrix[v][e] = int(math.ceil(space_matrix[v][e] / run.t))

    ls_result = ((np.matrix(phi).T * np.matrix(phi)).I * np.matrix(phi).T) * y
    ls = "complexity: " + str(int(ls_result[0]/run.t)) + "*V + " + str(int(ls_result[1]/run.t)) + "*E"

    time_data = [go.Surface(z=np.matrix(time_matrix).T)]
    space_data = [go.Surface(z=np.matrix(space_matrix).T)]

    graph_info = "a: " + run.a + ", V: " + str(run.v) + ", E: " + str(run.e) + \
                 ", SV: " + str(run.sv) + ", SE: " + str(run.se) + ", T: " + str(run.t)
    time_title = "Elapsed time [ms] <" + graph_info + "> -> " + ls
    space_title = "Stack dimension [byte] <" + graph_info + "> -> " + ls

    time_layout = go.Layout(title=time_title, autosize=True)
    space_layout = go.Layout(title=space_title, autosize=True)

    time_fig = go.Figure(data=time_data, layout=time_layout)
    space_fig = go.Figure(data=space_data, layout=space_layout)

    py.offline.plot(time_fig,
                    filename="time_complexity_graphs/time_complexity__A" + run.a + "V" + str(run.v) + "E" + str(
                        run.e) + ", SV: " + str(run.sv) + ", SE: " + str(run.se) + "T" + str(run.t) + ".html")
    py.offline.plot(space_fig,
                    filename="space_complexity_graphs/space_complexity__A" + run.a + "V" + str(run.v) + "E" + str(
        run.e) + ", SV: " + str(run.sv) + ", SE: " + str(run.se) + "T" + str(run.t) + ".html")


