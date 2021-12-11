from heapq import heapify
from os import path
import matplotlib.pyplot as plt
from dijkstra import dijkstra, path_to_str
from lp import *
from time import *
from graph import *
import numpy as np
import pandas as pd


def run(f: callable, args: any):
    start = time()
    out = f(*args)
    return (out, time() - start)


if __name__ == "__main__":

    START = "a"
    END = "g"
    ALPHA = 1
    OMEGA = 100

    # G = new_graph(N, P)
    G = read_file("./tests/graph_3.txt")


    g_tilde, starting_node, ending_node = delimit(transform_graph(G), START, END, ALPHA, OMEGA)
    print(f"From {starting_node} to {ending_node}")

    print("\n##############################\n")

    p, s, e  = arrivee_au_plus_tot(G, START, END, ALPHA, OMEGA)
    if p is not None :
        # p, s, e = pcc_out
        l, p_str = path_to_str(s, e, p)
        print("Arrivée au plus tot : ", p_str)
    else :
        print(f"No path found from {starting_node} to {ending_node}")
        sys.exit(0)

    print("\n##############################\n")

    p, s, e  = depart_au_plus_tard(G, START, END, ALPHA, OMEGA)
    if p is not None :
        l, p_str = path_to_str(s, e, p)
        print("Départ au plus tard : ", p_str)
    else :
        print(f"No path found from {starting_node} to {ending_node}")
        sys.exit(0)

    print("\n##############################\n")

    p, w, s, e  = plus_rapide_chemin(G, START, END, ALPHA, OMEGA)
    if p is not None :
        l, p_str = path_to_str(s, e, p)
        print("Chemin le plus rapide ", "(", w[e], ") :", p_str)
    else :
        print(f"No path found from {starting_node} to {ending_node}")
        sys.exit(0)

    print("\n##############################\n")

    p, w, s, e  = plus_court_chemin(G, START, END, ALPHA, OMEGA)
    if p is not None :
        l, p_str = path_to_str(extract_start(s,e,p), extract_end(e,p), p)
        print("Plus court chemin ", "(", w[e], ") :", p_str)
        graph_to_dot(g_tilde, './g_tilde.dot', p_str)
    else :
        print(f"No path found from {starting_node} to {ending_node}")
        sys.exit(0)

    print("\n##############################\n")

    lp = write_lp_constraints(convert_graph_into_matrix(g_tilde), starting_node, ending_node)
    cost, path = optimize_lp_and_pretty_format_result(*lp, starting_node)
    print("Résolution avec Gurobi ","(", cost ,") :", path )

