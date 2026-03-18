from random import randint
import numpy as np
import matplotlib.pyplot as ml
import networkx as nx
n = 15
A = []

for i in range(n):
    A.append([0 for g in range(n)])

for i in range(n):
    e = randint(0,i)
    for g in range(e):
        v = randint(0,g)
        A[i][v] = 1
        A[v][i] = 1
G = np.array(A) 
G = nx.from_numpy_array(G)
nx.draw(G, with_labels=True, node_color='lightblue', edge_color='gray', node_size=1000)
ml.show()