import plotly as py
import plotly.graph_objs as go


def get_heap(filename):
    f = open(filename, "r")

    fl = f.readlines()

    heap_B = [0]
    t = [0]

    i = 0
    for line in fl:
        if line[:11] == "mem_heap_B=":
            heap_B.append(int(line[11:]))
            t.append(i)
            i = i + 1

    maximums = [True] * len(heap_B)
    it = 0

    maximums[0] = False
    maximums[-1] = False

    correctRes = True

    while maximums.count(True) > 2 and correctRes:
        it = it + 1

        for i in range(1, len(heap_B) - 1):

            if not maximums[i]:
                continue

            p = heap_B[i - 1]
            v = heap_B[i]
            s = heap_B[i + 1]

            dvp = v - p
            dsv = s - v

            c = it / 10.0
            if not ((dvp >= 0) and (dsv < 0) and (-dsv > c * dvp)):
                maximums[i] = False

        if it > 10000:
            correctRes = False

    twoMax = []

    for i in range(len(heap_B)):
        if maximums[i]:
            twoMax.append(heap_B[i])

    return (t, heap_B, twoMax, correctRes)


(t, heap_B, twoMax, correct) = get_heap("pearce500x1000")

print twoMax

heap_data = [go.Scatter(x=t, y=heap_B)]

heap_layout = go.Layout(title="heap", autosize=True)

heap_fig = go.Figure(data=heap_data, layout=heap_layout)

py.offline.plot(heap_fig, filename="testPlot")
