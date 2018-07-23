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

    return {"peak_heap_B": peak_heap_B, "peak_heap_extra_B": peak_heap_extra_B, "peak_stacks_B": peak_stacks_B}
