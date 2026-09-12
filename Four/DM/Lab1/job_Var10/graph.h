#ifndef GRAPH_H
#define GRAPH_H
#include <stdio.h>

typedef struct Vertex Vertex;
typedef struct EdgeNode EdgeNode;

typedef struct Edge
{
    Vertex* vertex_1;
    Vertex* vertex_2;
    int weight;
} Edge;

typedef struct EdgeNode
{
    int ind;
    Edge* edge;
    EdgeNode* next;
} EdgeNode;
typedef struct Vertex
{
    int number;
    EdgeNode* edges;
} Vertex;


typedef struct Graph
{
    int order;
    Vertex* vertexes;
    int weight;
}Graph;







#endif /* GRAPH_H */