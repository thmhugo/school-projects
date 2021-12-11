import networkx as nx
from random import *
import sys
from dijkstra import dijkstra, path_to_str, extract_end, extract_start
from collections import defaultdict


def new_graph(n, p):
    G = {}
    G = defaultdict(list)
    for i in range(n):
        for j in range(i + 1, min(i + 10, n)):
            x = random()
            if x < p:
                G[(str(i))].append(
                    (str(j), float(randint(i, i + 20)), float(randint(1, 4)))
                )

    return G


# Read a file and create a graph from the informations extracted
def read_file(name):
    try:
        with open(name) as file:
            data = file.readlines()
            data = [line.strip() for line in data]
    except FileNotFoundError:
        print("File does not exist.")
        sys.exit(0)
    else:
        try:
            nodes_number = int(data[0])
            edges_number = int(data[1])
        except ValueError:
            print("The number of nodes and/or edges in the file are not integer.")
        else:
            G = defaultdict(lambda: None)
            try:
                # Add all the nodes to the graph
                for i in range(2, 2 + nodes_number):
                    G[data[i]] = []

                # Add all the edges to the graph
                for i in range(2 + nodes_number, edges_number + nodes_number + 2):
                    arc = data[i].strip("()").split(",")
                    u, v, t, l = arc[0], arc[1], arc[2], arc[3]
                    G[u].append((v, t, l))
            except:
                print("Incorrect file format.")
                sys.exit(0)
            return G


def read_console():
    try:
        nodes_number = int(input("How many nodes are in your graph ? "))
    except ValueError:
        print("The input was not a valid integer.")
    else:

        try:
            edges_number = int(input("How many edges are in your graph ? "))
        except ValueError:
            print("The input was not a valid integer.")
        else:
            G = defaultdict(lambda: None)
            for _ in range(nodes_number):
                u = input("Give a node to add to the graph : ")
                while u in G:
                    print("This node already is in the graph, add another.")
                    u = input("Give a node to add to the graph : ")
                G[u] = []

            for _ in range(edges_number):
                u = input("Give the start of a travel : ")
                ans = ""
                while u not in G and ans != "y":
                    ans = input(
                        "This input is not in the graph, do you want to add it ? (y or n) : "
                    )
                    if ans != "y":
                        u = input("Give the start of a travel : ")
                G[u] = []
                v = input("Give the end of an edge : ")
                while v not in G and ans != "y":
                    ans = input(
                        "This input is not in the graph, do you want to add it ? (y or n) : "
                    )
                    if ans != "y":
                        v = input("Give the end of the travel : ")
                G[v] = []
                t = int(input("Give the start time (integer) : "))
                while not isinstance(t, int):
                    t = input("The input is not an integer, give another start time : ")

                l = int(input("Give the travel time : "))
                while not isinstance(l, int):
                    l = input(
                        "The input is not an integer, give another travel time : "
                    )

                G[u].append((v, t, l))

            return G


def transform_graph(G):
    if G is None:
        return None
    Gbis = defaultdict(list)
    V = []
    Vin = defaultdict(list)
    Vout = defaultdict(list)

    for u in G.keys():
        for (v, t, s) in G[u]:
            start_time = int(t)
            travel_time = int(s)

            Vout[u].append((u, start_time))
            Vin[v].append((v, start_time + travel_time))

            Gbis[(u, start_time)].append((v, start_time + travel_time, travel_time))

            V.append((u, start_time))
            V.append((v, start_time + travel_time))
    # merges, remove duplicates, then sorts by key, then value
    # TODO : find a more efficient way
    V = sorted(list(dict.fromkeys(V)), key=lambda e: (e[0], e[1]))
    for i in range(len(V) - 1):
        if V[i][0] == V[i + 1][0]:
            Gbis[V[i]].append((*V[i + 1], 0))

    return Gbis

def _check_limits(alpha, omega, min_t, max_t, x,y, u):
    v, t = u
    if t >= alpha and t <= omega:
        if v == str(y) and t > max_t:
            max_t = t
        if v == str(x) and t < min_t:
            min_t = t
    return min_t, max_t

def delimit(G, x, y, alpha, omega):
    min_t = float("inf")
    max_t = float("-inf")
    for u in list(G.keys()):
        # Need to check the starting node that may not be in the successors.
        # (like ('a', 1) from the graph of 4th page of the subject)
        min_t, max_t = _check_limits(alpha, omega, min_t, max_t, x, y, u)
        delimited_gu = []
        for (v, _t, l) in G[u]:
        # If they are in, we check all their neighbors and do the same check
        # only keep the goo nodes and save the new starting and ending nodes.[u]
            if _t >= alpha and _t <= omega:
                if v == str(y) and _t > max_t:
                    max_t = _t
                if v == str(x) and _t < min_t:
                    min_t = _t
                delimited_gu.append((v, _t, l))
        G[u] = delimited_gu
        # nodes if they are not in the expected time interval
        _, t = u
        if t < alpha or t > omega:
            del G[u]

    # may suppress nodes without any edges
    return G, (x, min_t), (y, max_t)


def graph_to_dot(graph, file_path, path):
    nodes = path.split(" -> ")
    with open(file_path, "w") as f:
        f.write("strict digraph  {\n")
        f.write(f'"{nodes[len(nodes)-1]}" [color=red];\n')
        for u in graph.keys():
            s = f'"{u}" '
            if str(u) in nodes:
                s += "[color=blue]"
            s += ";\n"
            f.write(s)
        for u in graph.keys():
            for v, t, l in graph[u]:
                s = f'"{u}" -> "(\'{v}\', {t})" [ '
                if str(u) in nodes and f"('{v}', {t})" in nodes:
                    color = "blue" if l != 0 else "red"
                    s += f"color={color} "
                s += f"label={l}];\n"
                f.write(s)
        f.write("}")


def transpose(graph):
    # return the graph transposed of 'graph'
    transposed_graph = defaultdict(list)
    for u in graph.keys():
        for (s, t, l) in graph[u]:
            transposed_graph[(s, t)].append((*u, l))
    return transposed_graph


def get_v_out(G, node):
    # return all nodes that has and edge with 'node' with a weigth = 0
    v_out = []
    for u, t, l in G[node]:
        if l == 0:
            v_out.append((u, t))
    for (_u, _t) in v_out:
        v_out += get_v_out(G, (_u, _t))
    return v_out


def get_all_starting_nodes(G, starting_node):
    # return all nodes in G tilde associated to the starting node in G.
    return list(dict.fromkeys([starting_node] + get_v_out(G, starting_node)))

def bfs(G, s) :
    P, Q = defaultdict(lambda: None),[s]
    while Q :
        u = Q.pop()
        if G[u] is not None:
            for v in G[u] :
                _v, t, _ = v
                if (_v, t) in P : continue
                P[(_v, t)]=u
                Q.append((_v, t))
    return P


def depart_au_plus_tard(G, start, end, alpha, omega):
    g_tilde, starting_node, ending_node = delimit(
        transform_graph(G), start, end, alpha, omega
    )
    lastest_start = (None, float("-inf"))
    lastest_path = None
    starting_nodes = get_all_starting_nodes(g_tilde, starting_node)
    for n in starting_nodes:
        path = bfs(g_tilde, n)
        if path[ending_node] is not None:
            _, t = n
            if t > lastest_start[1]:
                lastest_start = n
                lastest_path = path

    return lastest_path, lastest_start, ending_node


def arrivee_au_plus_tot(G, start, end, alpha, omega):
    g_tilde, starting_node, ending_node = delimit(
        transform_graph(G), start, end, alpha, omega
    )
    g_t = transpose(g_tilde)
    earliest_start_t = (None, float("inf"))
    earliest_path_t = None
    earliest_starting_nodes_t = get_all_starting_nodes(g_t, ending_node)

    for n in earliest_starting_nodes_t:
        path_t = bfs(g_t, n)
        # a path n (an ending node in g_t) to starting_node (a starting node in g)
        if path_t[starting_node] is not None:
            _, t = n
            if t < earliest_start_t[1]:
                earliest_start_t = n
                earliest_path_t = path_t

    return earliest_path_t, earliest_start_t, starting_node


def plus_rapide_chemin(G, start, end, alpha, omega):
    g, starting_node, ending_node = delimit(
        transform_graph(G), start, end, alpha, omega
    )
    quicker_start_d = None
    quicker_end_d = None
    quicker_path_d = None
    quicker_weights = None
    quicker_duration = float("inf")
    quicker_starting_nodes_d = get_all_starting_nodes(g, starting_node)
    for n in quicker_starting_nodes_d:
        path_d, weights = dijkstra(g, n)
        real_ending_node = extract_end(ending_node, path_d)
        real_starting_node = extract_start(n, real_ending_node, path_d)
        if path_d[real_ending_node] is not None:
            _, s = real_starting_node
            _, e = real_ending_node
            d = e - s
            if d < quicker_duration:
                quicker_duration = d
                quicker_start_d = real_starting_node
                quicker_end_d = real_ending_node
                quicker_path_d = path_d
                quicker_weights = weights
    return quicker_path_d, quicker_weights, quicker_start_d, quicker_end_d


def plus_court_chemin(G, start, end, alpha, omega):
    g, starting_node, ending_node = delimit(
        transform_graph(G), start, end, alpha, omega
    )
    return *dijkstra(g, starting_node), starting_node, ending_node
