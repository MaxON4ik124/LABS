#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
typedef struct BotGraph
{
    int x;
    int y;
    int* nextinds;
    int next_index;
    struct BotGraph* next[8];
} BotGraph;
void printBotGraph(BotGraph* graph)
{
    printf("X:%d Y:%d\n", graph->x, graph->y);
    int* i = graph->nextinds;
    while(*i != -1)
    {
        printf("N:%d  ", *i);
        *i++;
    }
    printf("%d\n", graph->next_index);
}
void CreateBotGraph(BotGraph** head)
{
    FILE* graph_list = fopen("input.txt", "r");
    char str[512];
    BotGraph *current = (BotGraph*)malloc(sizeof(BotGraph));
    *head = current;
    while (fgets(str, 512, graph_list) != NULL)
    {
        sscanf(str, "{%d, %d}", &current->x, &current->y);
        char* part = strtok(str, " ");
        part = strtok(NULL, " ");
        int* nextindexes = (int*)malloc(sizeof(int*));
        int* beg = nextindexes;
        int i = 0;
        while(part != NULL)
        {
            int val = atoi(part);
            *nextindexes = val;
            *nextindexes++;
            part = strtok(NULL, " ");
            i++;
        }
        *nextindexes = -1;
        *nextindexes++;
        current->next_index = i;
        current->nextinds = beg;
        current->next_index = 0;
        current->next[0] = (BotGraph*)malloc(sizeof(BotGraph));
        current = current->next[0];
    }
    current->next[0] = NULL;
}
// void setBotGraph(BotGraph* src, BotGraph** head)
// {
//     BotGraph* current = src;
// }
int main()
{
    BotGraph *source = (BotGraph*)malloc(sizeof(BotGraph));
    BotGraph *graph = (BotGraph*)malloc(sizeof(BotGraph));
    CreateBotGraph(&source);
    printBotGraph(source);
    graph = source;
}