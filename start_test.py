import math
import os

N = 10  # Maximum number of nodes
M = 10  # Maximum number of arcs
S = 5  # Stride
T = 1  # Number of trials for each (edge, vertex) configuration

uN = int(math.floor(N / S))  # Unitary M
uM = int(math.floor(M / S))  # Unitary M

algorithm = 'p'

# Declaring the uN x uM matrix storing the time measurements
time_matrix = [0] * uN
for i in range(uN):
    time_matrix[i] = [0] * uM

# Sum the T time measurements taken from each (vertex,edge) configuration
for n in range(0, uN):
    for m in range(0, uM):
        for t in range(T):
            cmd_string = "./cmake-build-debug/AAProject " + str((n + 1) * S) + " " + str((m + 1) * S) + " " + algorithm
            time_matrix[n][m] += int(os.popen(cmd_string).read())

# Divide the whole element of the matrix by T (so to get the average time)
for n in range(0, uN):
    for m in range(0, uM):
        time_matrix[n][m] = int(math.ceil(time_matrix[n][m] / T))

print time_matrix
