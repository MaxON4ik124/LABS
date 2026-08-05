#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "graph.h"

int var;
int start;

void InitVertex(Vertex* vertex, int num)
{
    vertex->number = num;
    vertex->edges = NULL;
}
void InitEdge(Edge* edge, Vertex* v1, Vertex* v2, int weight)
{
    edge->vertex_1 = v1;
    edge->vertex_2 = v2;
    edge->weight = weight; 
}
void BindEdge2Vertex(Vertex* vertex, Edge* edge)
{
    EdgeNode* node = malloc(sizeof(EdgeNode));
    node->edge = edge;
    node->next = vertex->edges;
    vertex->edges = node;
}
Graph* InitGraphFromFile(char* filename)
{
    Graph* graph = malloc(sizeof(Graph));
    FILE* file = fopen(filename, "r");
    int n;
    fscanf(file, "%d %d", &n, &var);
    if(var == 2) 
        fscanf(file,"%d", &start);
    graph->order = n;
    graph->vertexes = malloc(n * sizeof(Vertex));
    for(int i = 1;i < n+1;i++) 
        InitVertex(&graph->vertexes[i-1], i);
    int w = -1;
    for(int i = 1;i < n+1;i++)
    {
        for(int g = 1;g < n+1;g++)
        {
            fscanf(file, "%d", &w);
            if(w != 0 && g > i)
            {
                Edge* edge = malloc(sizeof(Edge));
                InitEdge(edge, &graph->vertexes[i-1], &graph->vertexes[g-1], w);
                BindEdge2Vertex(&graph->vertexes[i-1], edge);
                BindEdge2Vertex(&graph->vertexes[g-1], edge);
            }
        }
    }
    return graph;
}

void PrintAllEdges(FILE* stream, EdgeNode* head)
{
    EdgeNode* cur = head;
    while(cur != NULL)
    {
        fprintf(stream, "(%d - %d) ", cur->edge->vertex_1->number, cur->edge->vertex_2->number, cur->edge->weight);
        cur = cur->next;
    }
}
void SwapNodes(EdgeNode* e1, EdgeNode* e2)
{
    Edge* temp = e1->edge;
    e1->edge = e2->edge;
    e2->edge = temp;
}
EdgeNode* GetTail(EdgeNode* head)
{
    EdgeNode* tail = head;
    while(tail->next != NULL)
        tail = tail->next;
    return tail;
}
EdgeNode* GetPrev(EdgeNode* head, EdgeNode* node)
{
    if (head == NULL || head == node) return NULL;
    while (head->next != NULL && head->next != node) head = head->next;
    if (head->next == node) return head;
    return NULL;
}
void SortEdgesByWeight(EdgeNode* head, EdgeNode* tail)
{
    if (head == NULL || tail == NULL || head == tail)
        return;
    int pivot = tail->edge->weight;
    EdgeNode* INode = NULL;
    EdgeNode* JNode = head;
    while(JNode != tail)
    {
        if(JNode->edge->weight < pivot)
        {
            if(INode == NULL) INode = head;
            else INode = INode->next;
            SwapNodes(INode, JNode);
        }
        JNode = JNode->next;
    }
    if(INode == NULL) INode = head;
    else INode = INode->next;
    SwapNodes(INode, JNode);
    EdgeNode* prevPivot = GetPrev(head, INode);

    if (prevPivot != NULL)
        SortEdgesByWeight(head, prevPivot);

    if (INode != NULL && INode != tail && INode->next != NULL)
        SortEdgesByWeight(INode->next, tail);
}
EdgeNode* ExtractEdgeArr(Graph* graph)
{
    EdgeNode* head = NULL;
    int I = 0;
    for(int i = 0;i < graph->order;i++)
    {
        EdgeNode* cur = graph->vertexes[i].edges;
        while(cur != NULL)
        {
            Edge* edge = cur->edge;
            if(edge->vertex_1 == &graph->vertexes[i])
            {
                EdgeNode* new_node = malloc(sizeof(EdgeNode));
                new_node->edge = edge;
                new_node->next = head;
                head = new_node;
                new_node->ind = I;
                I++;
            }
            cur = cur->next;
        }
    }
    return head;
}
int CheckLoopBFS(Graph* G, EdgeNode* node)
{
    int V = node->edge->vertex_1->number-1;
    int U = node->edge->vertex_2->number-1;

    int n = G->order;
    int visited[n];
    for(int i = 0;i < n;i++) visited[i] = 0;
    int Q[n];
    int front = 0, rear = 0;
    Q[rear++] = V;
    visited[V] = 1;
    while(front < rear)
    {
        int v = Q[front++];
        EdgeNode* cur = G->vertexes[v].edges;
        while(cur != NULL)
        {
            Edge* edge = cur->edge;
            int sosed = edge->vertex_1->number-1 == v ? edge->vertex_2->number-1 : edge->vertex_1->number-1;
            if(sosed == U) return 1;
            if(visited[sosed] == 0)
            {
                visited[sosed] = 1;
                Q[rear++] = sosed;
            }
            cur = cur->next;
        }
    }
    return 0;
}
Graph* BuildSkeletonByKruckal(EdgeNode* E, int order)
{
    Graph* Skeleton = malloc(sizeof(Graph));
    Skeleton->order = order;
    Skeleton->vertexes = malloc(order * sizeof(Vertex));
    Skeleton->weight = 0;
    for(int i = 0;i < order;i++) InitVertex(&Skeleton->vertexes[i], i+1);

    EdgeNode* cur = E;
    int edge_cnt = 0;

    while(cur != NULL && edge_cnt < order-1)
    {
        int v1 = cur->edge->vertex_1->number-1;
        int v2 = cur->edge->vertex_2->number-1;
        if(CheckLoopBFS(Skeleton, cur) == 0)
        {
            Edge* edge = malloc(sizeof(Edge));
            InitEdge(edge, &Skeleton->vertexes[v1], &Skeleton->vertexes[v2], cur->edge->weight);
            BindEdge2Vertex(&Skeleton->vertexes[v1], edge);
            BindEdge2Vertex(&Skeleton->vertexes[v2], edge);
            Skeleton->weight += cur->edge->weight;
            edge_cnt++;
        }
        cur = cur->next;
    }
    return Skeleton;
}
void CreateOutputGraph(Graph* G, char* filename)
{
    FILE* output = fopen(filename, "w");
    fprintf(output, "%d\n", G->weight);
    for(int i = 0;i < G->order;i++)
    {
        for(int j = 0;j < G->order;j++)
        {
            int w = 0;
            if(i != j)
            {
                EdgeNode* cur = G->vertexes[i].edges;
                while(cur != NULL)
                {
                    int v1 = cur->edge->vertex_1->number-1;
                    int v2 = cur->edge->vertex_2->number-1;
                    if((v1 == i && v2 == j) || (v1 == j && v2 == i))
                    {
                        w = 1;
                        break;
                    }
                    cur = cur->next;
                }
            }
            fprintf(output, "%d, ", w);
        }
        fprintf(output, "\n");
    }
}
int main()
{
    Graph* G = InitGraphFromFile("input_10.in");
    EdgeNode* E = ExtractEdgeArr(G);
    SortEdgesByWeight(E, GetTail(E));
    Graph* Skeleton = BuildSkeletonByKruckal(E, G->order);
    CreateOutputGraph(Skeleton, "input_10.out");
}