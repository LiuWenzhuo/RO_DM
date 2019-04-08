# -*- coding: utf-8 -*-
"""
Created on Tue Apr  9 00:44:19 2019

@author: Lucil
"""

import networkx as nx
import numpy as np
import matplotlib.pyplot as plt
from random import shuffle
from community import community_louvain
from matplotlib import colors as mcolors

def add_edge(p, G, s, t):
    p_r = np.random.uniform(0, 1)
    G_new = G
    if p_r<=p:
        G_new.add_edge(s, t)
    return G_new

def create_Graph (p, q):
    G = nx.Graph()
    G.add_nodes_from(range(400))
    for i in range(99):
        for j in range(i+1, 100):
            G = add_edge(p, G, i, j)
            G = add_edge(p, G, i+100, j+100)
            G = add_edge(p, G, i+200, j+200)
            G = add_edge(p, G, i+300, j+300)
    for i in range(100):
        for j in range(100, 400):
            G = add_edge(q, G, i, j)
    for i in range(100, 200):
        for j in range(200, 400):
            G = add_edge(q, G, i, j)
    for i in range(200, 300):
        for j in range(300, 400):
            G = add_edge(q, G, i, j)
    return G

def LabelPropagation(G):
    nodes = list(G.nodes)
    dic = {}
    for i, node in enumerate(nodes):
        dic[node] = i
    c = 1
    while (c != 0):
        shuffle(nodes)
        for node in nodes:
            c = 0
            neig_labels = [dic[n] for n in G.neighbors(node)]
            feq_label = max(set(neig_labels), key=neig_labels.count)
            if (dic[node] != feq_label):
                c += 1
                dic[node] = feq_label
    return dic

def similarity(G, x, y):
    neig_x = set(G.neighbors(x))
    neig_y = set(G.neighbors(y))
    inter = neig_x&neig_y
    if (len(inter)==0):
        return 0
    else:
        return len(inter)/(len(neig_x|neig_y))
    
def create_community(G, threshold):
    nodes = list(G.nodes)
    dic = {}
    for i, node in enumerate(nodes):
        dic[node] = i
    c = 1
    while (c != 0):
        shuffle(nodes)
        for node in nodes:
            c = 0
            similar_nodes =[n for n in nodes if n!=node and 
                            similarity(G, n, node)>threshold]
            similar_labels = [dic[n] for n in similar_nodes]
            feq_label = max(set(similar_labels), key=similar_labels.count)
            if (dic[node] != feq_label):
                c += 1
                dic[node] = feq_label
    return dic

def draw_com(G, partition):
    size = len(set(partition.values()))
    colors = dict(mcolors.BASE_COLORS, **mcolors.CSS4_COLORS)
    pos = nx.spring_layout(G)
    count = 0
    color = list(colors.keys())[:(size+1)]
    for com in set(partition.values()) :
        count = count + 1
        list_nodes = [nodes for nodes in partition.keys()
                                if partition[nodes] == com]
        nx.draw_networkx_nodes(G, pos, list_nodes, node_size = 20,
                                node_color = color[count])


    nx.draw_networkx_edges(G,pos, alpha=0.5)
    plt.show()
    
if __name__ == '__main__':
    G = nx.Graph()
    G.add_nodes_from(range(400))
    G = create_Graph(0.3, 0.1)
    nx.draw_networkx(G, arrows=False, with_labels=False, node_size = 40, alpha = 0.5)
    lab = LabelPropagation(G)
    draw_com(G, lab)
    sim = create_community(G, 0.13)
    draw_com(G, sim)
    partition = community_louvain.best_partition(G)
    draw_com(G, partition)

