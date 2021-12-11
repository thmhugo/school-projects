from gurobipy import *
from collections import defaultdict


def convert_graph_into_matrix(graph):
    adj_mat = defaultdict(lambda: defaultdict(lambda: float("inf")))
    for n in graph:
        for v, t, l in graph[n]:
            adj_mat[n][(v, t)] = l
    return adj_mat


def write_lp_constraints(graph, start, end):
    m = Model("fig.1")
    x = {}
    obj = LinExpr()
    for i in graph:
        for j in graph.get(i):
            if graph.get(i).get(j) is not None:
                v = m.addVar(vtype=GRB.BINARY, name="x" + str(i) + "," + str(j))
                x[str(i) + "->" + str(j)] = v
                obj += graph.get(i).get(j) * v
            else:
                del graph[i][j]
    m.setObjective(obj, GRB.MINIMIZE)
    # Starting vertice constraint
    m.addConstr(
        sum(
            x[str(start) + "->" + str(i)]
            for i in graph[start]
            if graph.get(start).get(i) is not None
        )
        == 1
    )
    # Ending vertice constraint
    m.addConstr(
        sum(
            x[str(end) + "->" + str(k)]
            for k in graph[end]
            if graph.get(end).get(k) is not None
        )
        - sum(
            x[str(k) + "->" + str(end)]
            for k in graph
            if graph.get(k).get(end) is not None
        )
        == -1
    )

    for i in graph:
        if i != start and i != end:
            m.addConstr(
                sum(
                    (x[str(i) + "->" + str(k)])
                    for k in graph
                    if graph.get(i).get(k) is not None
                )
                - sum(
                    x[str(k) + "->" + str(i)]
                    for k in graph.keys()
                    if graph.get(k).get(i) is not None
                )
                == 0
            )
    m.update()
    return (m, x)


def optimize_lp_and_pretty_format_result(m: Model, x: dict, d: int) -> int:
    m.optimize()
    c = 0
    path = " | "
    try:
        for v in x.values():
            if v.x == 1.0:
                c += 1
                path +=  v.varName + " | "
        return c, path
    except AttributeError:
        return float('inf'), "No solution found for this LP."
