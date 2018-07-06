import math
import os

import plotly as py
import plotly.graph_objs as go
import progressbar

V = 1000  # Maximum number of vertices
E = 1000  # Maximum number of edges
S = 10  # Stride
T = 10  # Number of trials for each (edge, vertex) configuration

uV = int(math.floor(V / S))  # Unitary V
uE = int(math.floor(E / S))  # Unitary E

algorithm = 'p'

# Declaring the uV x uE matrix storing the time measurements
time_matrix = [0] * uV
for i in range(uV):
    time_matrix[i] = [0] * uE

bar = progressbar.ProgressBar(maxval=uV * uE * T,
                              widgets=[progressbar.Bar('=', '[', ']'), ' ', progressbar.Percentage()])

bar.start()
# Sum the T time measurements taken from each (vertex,edge) configuration
for v in range(0, uV):
    for e in range(0, uE):
        for t in range(T):
            cmd_string = "./cmake-build-debug/AAProject " + str((v + 1) * S) + " " + str((e + 1) * S) + " " + algorithm
            time_matrix[v][e] += int(os.popen(cmd_string).read())
            bar.update((v + 1) * (e + 1) * (t + 1))
bar.finish()

# Divide the whole element of the matrix by T (so to get the average time)
for v in range(0, uV):
    for e in range(0, uE):
        time_matrix[v][e] = int(math.ceil(time_matrix[v][e] / T))

data = [go.Surface(z=time_matrix)]

graph_info = "a: " + algorithm + ", V: " + str(V) + ", E: " + str(E) + ", S: " + str(S) + ", T: " + str(T)
title = "Time performance [" + graph_info + "]"

layout = go.Layout(title=title, autosize=True)
fig = go.Figure(data=data, layout=layout)

py.offline.plot(fig, filename="time_complexity_graphs/time_complexity__A" + algorithm + "V" + str(V) + "E" + str(
    E) + "S" + str(S) + "T" + str(T) + ".html")
