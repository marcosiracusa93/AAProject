import math
from subprocess import check_output

import plotly as py
import plotly.graph_objs as go
import progressbar
import numpy as np


class Run:
    def __init__(self, v=0, e=0, sv=1, se=100, t=1, a=''):
        self.v = v  # Maximum number of vertices
        self.e = e  # Maximum number of edges
        self.sv = sv  # Stride
        self.se = se
        self.t = t  # Number of trials for (edge, vertex) configuration
        self.a = a  # Algorithm


runs = [Run(v=50, e=300, sv=1, se=3, t=10, a='n')]

for run in runs:
    uV = int(math.floor(run.v / run.sv))  # Unitary V
    uE = int(math.floor(run.e / run.se))  # Unitary E

    # Declaring the uV x uE matrix storing the time measurements
    time_matrix = [0] * uV
    for i in range(uV):
        time_matrix[i] = [0] * uE

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

                output = int(check_output(["./cmake-build-debug/AAProject", str((v + 1) * run.sv), str((e + 1) * run.se), run.a]))

                # updating matrices for least squares
                phi[(uE*v)+e, 0] = (v + 1) * run.sv
                phi[(uE*v)+e, 1] = (e + 1) * run.se
                y[(uE*v)+e] += output

                # updating time matrix
                time_matrix[v][e] += output

                bar.update((v + 1) * (e + 1) * (t + 1))
    bar.finish()

    # Divide the whole element of the matrix by run.t (so to get the average time)
    for v in range(0, uV):
        for e in range(0, uE):
            time_matrix[v][e] = int(math.ceil(time_matrix[v][e] / run.t))

    ls_result = ((np.matrix(phi).T * np.matrix(phi)).I * np.matrix(phi).T) * y
    ls = "complexity: " + str(int(ls_result[0]/run.t)) + "*V + " + str(int(ls_result[1]/run.t)) + "*E"

    data = [go.Surface(z=np.matrix(time_matrix).T)]

    graph_info = "a: " + run.a + ", V: " + str(run.v) + ", E: " + str(run.e) + \
                 ", SV: " + str(run.sv) + ", SE: " + str(run.se) + ", T: " + str(run.t)
    title = "Time performance [" + graph_info + "] -> " + ls

    layout = go.Layout(title=title, autosize=True)

    fig = go.Figure(data=data, layout=layout)

    py.offline.plot(fig, filename="time_complexity_graphs/time_complexity__A" + run.a + "V" + str(run.v) + "E" + str(
        run.e) + ", SV: " + str(run.sv) + ", SE: " + str(run.se) + "T" + str(run.t) + ".html")



