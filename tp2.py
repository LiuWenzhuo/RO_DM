# -*- coding: utf-8 -*-
"""
Created on Mon Apr  8 22:07:43 2019

@author: Lucil
"""

import numpy as np


#out degree of ecah nodes in the graph
def out_degree(edges):
    d = {}
    for edge in edges:
        if edge[0] in d:
            d[edge[0]] += 1
        else:
            d[edge[0]] = 1
        if edge[1] not in d:
            d[edge[1]] = 0
    return d

#in degree of each nodes in the graph
def in_degree(edges):
    d = {}
    for edge in edges:
        if edge[1] in d:
            d[edge[1]] += 1
        else:
            d[edge[1]] = 1
        if edge[0] not in d:
            d[edge[0]] = 0
    return d

#find dead ends from the out degree of each nodes
def dead_ends(d):
    p = []
    for i in d:
        if d[i] ==0:
            p.append(i)
    return p

#find transition matrix of a graph, we only calculate values for each edges,
    #so T has 2*m non zoro values. 
# For values concerning deadends, they are constant (1/n), we don't need to
    #save them in matrix T.
def transMat(edges, d):
    T={}
    for edge in edges:
        T[tuple([edge[1], edge[0]])] = 1/d[edge[0]]
    return T

# prod compute the product of transition matrix and vector P.
def Prod(T, P, deadends):
    
    s = sum([P[i] for i in deadends])
    V = dict.fromkeys(P.keys(), s)
    for edge in T:
        V[edge[0]] += T[edge] * P[edge[1]]
    for node in deadends:
        V[node] = V[node] - P[node]
    return V

def PageRank(T, alpha, t, d, deadends):
    P = dict.fromkeys(d.keys(), 1/len(d))
    n = len(d)
    for i in range(t):
        P = Prod(T, P, deadends)
        for node in P:
            P[node] = (1-alpha)*P[node] +alpha/len(d)
        s = sum(P.values())
        for node in P:
            P[node] += (1-s)/n
        print(sorted(P, key=P.get, reverse=True)[:5])
    return P

def k_core(listedges, degree):
    c = 0
    i = len(degree)
    dic_1 = {}
    dic_2 = {}
    i = len(degree)
    while (listedges!={}):
        v = min(degree, key = degree.get)
        c = max(c, degree[v])
        nodes = listedges.pop(v, None)
        degree.pop(v, None)
        for l in listedges:
            listedges[l] = [a for a in listedges[l] if a != v]
        for node in nodes:
            degree[node] = degree[node] - 1
        dic_1[v] = c
        dic_2[v] = i
        i = i - 1
    return dic_1, dic_2 

def create_AdjacencyList(filename):
    listedges = {}
    with open(filename) as fp:
        for line in fp:
            if "#" not in line:
                edge = [int(i) for i in line.split()]
                if edge[0] not in listedges:
                    listedges[edge[0]] = [edge[1]]
                else:
                    listedges[edge[0]].append(edge[1])
                if edge[1] not in listedges:
                    listedges[edge[1]] = [edge[0]]
                else:
                    listedges[edge[1]].append(edge[0])
    return listedges

if __name__ == '__main__':
    l = []
    with open("alr21--dirLinks--enwiki-20071018.txt") as fp:
        for line in fp:
            int_list = [int(i) for i in line.split()]
            l.append(int_list)
    d = out_degree(l)
    T = transMat(l, d)
    deadends = dead_ends(d)
    P=PageRank(T, 0.15, 10, d, deadends)
    
    l_new = create_AdjacencyList('email-eu-core')
    degree = {}
    for node in l_new:
        degree[node] = len(l_new[node])
    dic_1, dic_2 = k_core(l_new, d)