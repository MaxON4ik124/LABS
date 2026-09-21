#ifndef MAP_H
#define MAP_H

#include "main.h"

extern int map[MAP_HEIGHT][MAP_WIDTH];

extern int texture_map[MAP_HEIGHT][MAP_WIDTH];

bool check_map_collision(float x, float y, float radius);
void find_spawn_point(float* x_pos, float* y_pos, int tankType);
void generate_map(char* filename);
int load_graph(const char* filename, BotGraph** out_nodes);
void free_graph(BotGraph* nodes, int count);

#endif
