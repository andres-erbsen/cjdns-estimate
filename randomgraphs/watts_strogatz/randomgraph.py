#!/usr/bin/env python
# -*- coding: utf-8 -*-

import networkx as nx
import sys
from random import randint

def output_graph(G):
    out_str = ""
    for node in G.nodes():
        out_str += str(node)+' '+str(randint(0,2147483647))+'\n'
    out_str += "#\n"
    for edge in G.edges():
        out_str += str(edge[0])+' '+str(edge[1])+' '+str(randint(0,1000))+'\n'
    print out_str

if __name__ == "__main__":
    n = int(sys.argv[1])
    m = 5
    p = 0.2
    G = nx.watts_strogatz_graph(n, m, p)
    while not nx.is_connected(G):
        G = nx.watts_strogatz_graph(n, m, p)
    output_graph(G)
