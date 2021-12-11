from collections import defaultdict
from heapq import *


def dijkstra(graph, s):
    seen = set()
    g = defaultdict(list)
    for u in graph.keys():
        for v, t, l in graph[u]:
            g[u].append((l, (v, t)))
    weights = defaultdict(lambda: float("inf"))
    weights[s] = 0.0
    path = defaultdict(lambda: None)
    queue = []
    heapify(queue)
    heappush(queue, (0, s))
    while queue:
        c, u = heappop(queue)
        seen.add(u)
        for w, v in g[u]:
            if v not in seen:
                f = c + w
                if f < weights[v]:
                    weights[v] = f
                    path[v] = u
                    heappush(queue, (f, v))
    return path, weights


def path_to_str(start, end, paths):
    if end[1] == float("-inf"):
        return ""
    dad = paths[end]
    path = str(end)
    path_length = 0

    while dad is not None and dad is not start  :
        path_length += 1
        path = str(dad) + " -> " + path
        dad = paths[dad]
    return path_length + 1, str(dad) + " -> " + path


def extract_start(start, end, path):
    dad = path[end]
    if path[dad] is None:
        return dad
    while path[dad][0] != start[0]:
        dad = path[dad]
    return path[dad]



def extract_end(end, path):
    dad = path[end]
    if dad[0] != end[0]:
        return end
    while path[dad][0] == dad[0]:
        dad = path[dad]
    return dad
