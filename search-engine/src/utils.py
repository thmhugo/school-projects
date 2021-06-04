import time


def get_process_time(f):
    """"Calculates the process time of `f` call"""
    t1 = time.process_time()
    res = f()
    elapsed_time = abs(time.process_time() - t1)
    return round(elapsed_time, 4), res
