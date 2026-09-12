#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

typedef struct BotGraph
{
    int x;
    int y;

    int* nextinds;       
    int next_index;      

    struct BotGraph* next[8];  
} BotGraph;

void parse_node(char* line, BotGraph* node)
{
    sscanf(line, " { %d , %d } ", &node->x, &node->y);

    char* p = strchr(line, '}');
    if (!p) {
        node->next_index = 0;
        return;
    }
    p++;
    node->nextinds = malloc(sizeof(int) * 8);
    node->next_index = 0;

    while (*p)
    {
        while (*p && isspace(*p))
            p++;

        if (isdigit(*p))
        {
            int value = strtol(p, &p, 10);
            node->nextinds[node->next_index++] = value;
        }
        else
        {
            p++;
        }
    }
}
void link_nodes(BotGraph* nodes, int count)
{
    for (int i = 0; i < count; i++)
    {
        BotGraph* cur = &nodes[i];

        for (int j = 0; j < cur->next_index; j++)
        {
            int index = cur->nextinds[j];

            if (index < 0 || index >= count) {
                cur->next[j] = NULL;
            } else {
                cur->next[j] = &nodes[index];
            }
        }
    }
}

int load_graph(const char* filename, BotGraph** out_nodes)
{
    FILE* f = fopen(filename, "r");
    if (!f) {
        perror("Failed to open file");
        return -1;
    }

    
    int count = 0;
    char buffer[256];

    while (fgets(buffer, sizeof(buffer), f))
        count++;

    rewind(f);
 
    BotGraph* nodes = calloc(count, sizeof(BotGraph));
    int i = 0;
    while (fgets(buffer, sizeof(buffer), f))
    {
        buffer[strcspn(buffer, "\n")] = 0;
        parse_node(buffer, &nodes[i]);
        i++;
    }

    fclose(f);

    link_nodes(nodes, count);

    *out_nodes = nodes;
    return count;
}


// int main()
// {
//     BotGraph* nodes;
//     int count = load_graph("input.txt", &nodes);

//     if (count < 0) return 1;

//     printf("Loaded %d nodes\n", count);

//     for (int i = 0; i < count; i++)
//     {
//         printf("Node %d: x=%d y=%d next: ", i, nodes[i].x, nodes[i].y);
//         for (int j = 0; j < nodes[i].next_index; j++)
//         {
//             int idx = nodes[i].nextinds[j];
//             printf("%d(x=%d,y=%d) ", idx, nodes[i].next[j]->x, nodes[i].next[j]->y);
//         }
//         printf("\n");
//     }

//     return 0;
// }
