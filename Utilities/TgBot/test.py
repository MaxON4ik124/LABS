import matplotlib.pyplot as plt
import networkx as nx
from trees import *
class Node:
    def __init__(self, val, left=None, right=None):
        self.val = val
        self.left = left
        self.right = right

def add_edges(G, node, pos={}, x=0, y=0, dx=1.0):
    if node is None:
        return
    G.add_node(node.value)
    pos[node.value] = (x, y)
    if node.left:
        G.add_edge(node.value, node.left.value)
        add_edges(G, node.left, pos, x - dx, y - 1, dx / 2)
    if node.right:
        G.add_edge(node.value, node.right.value)
        add_edges(G, node.right, pos, x + dx, y - 1, dx / 2)
    return pos

def draw_tree(root):
    G = nx.DiGraph()
    pos = add_edges(G, root)
    nx.draw(G, pos, with_labels=True, arrows=False, node_size=1000, node_color='lightgreen', font_size=14)
    plt.show()

# Пример дерева

tree = RedBlackTree()
els = [10, 20, 40, 30, 12, 15]
for el in els:
    tree.insert(el)
draw_tree(tree.root)
7203298205:AAF3saTT5Z4wqRsh6zdokbNd1nGA0EMxIS0