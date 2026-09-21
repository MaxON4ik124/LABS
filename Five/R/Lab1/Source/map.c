#include "main.h"
#include <ctype.h>


int map[MAP_HEIGHT][MAP_WIDTH] = { 0 };


int texture_map[MAP_HEIGHT][MAP_WIDTH] = { 0 };


void generate_map(char* filename)
{
    memset(map, TILE_EMPTY, sizeof(map));
    for (int y = 0; y < MAP_HEIGHT; y++) {
        for (int x = 0; x < MAP_WIDTH; x++) {
            if (x == 0 || y == 0 || x == MAP_WIDTH - 1 || y == MAP_HEIGHT - 1) {
                map[y][x] = TILE_WALL;
            }
        }
    }
    serial();
    FILE* mapfile = fopen(filename, "r");
    for (int y = 1; y < MAP_HEIGHT - 1; y++) {
        for (int x = 1; x < MAP_WIDTH - 1; x++) {
            fscanf(mapfile, "%d", &map[y][x]);
            texture_map[y][x] = rand() % 4;
        }
    }
    fclose(mapfile);
}


bool check_map_collision(float x, float y, float radius) {
    int tile_size = TILE_SIZE;


    int start_x = (int)((x - radius) / tile_size);
    int end_x = (int)((x + radius) / tile_size);
    int start_y = (int)((y - radius) / tile_size);
    int end_y = (int)((y + radius) / tile_size);

    for (int ty = start_y; ty <= end_y; ty++) {
        for (int tx = start_x; tx <= end_x; tx++) {
            if (tx >= 0 && tx < MAP_WIDTH && ty >= 0 && ty < MAP_HEIGHT) {
                if (map[ty][tx] == TILE_WALL || map[ty][tx] == TILE_BREAKABLE) {
                    float tile_center_x = tx * tile_size + tile_size / 2;
                    float tile_center_y = ty * tile_size + tile_size / 2;

                    float closest_x = fmaxf(tile_center_x - tile_size / 2, fminf(x, tile_center_x + tile_size / 2));
                    float closest_y = fmaxf(tile_center_y - tile_size / 2, fminf(y, tile_center_y + tile_size / 2));

                    float distance_x = x - closest_x;
                    float distance_y = y - closest_y;
                    float distance_squared = distance_x * distance_x + distance_y * distance_y;

                    if (distance_squared < radius * radius) {
                        return true;
                    }
                }
            }
        }
    }

    return false;
}

void find_spawn_point(float* x_pos, float* y_pos, int tankType) {
    for (int y = 1; y < MAP_HEIGHT - 1; y++)
    {
        for (int x = 1; x < MAP_WIDTH - 1; x++)
        {
            if (map[y][x] == 10 && tankType == 0)
            {
                *x_pos = x * TILE_SIZE;
                *y_pos = y * TILE_SIZE;
                map[y][x] = TILE_EMPTY;
                return;
            }
            if (map[y][x] == 11 && tankType == 1)
            {
                *x_pos = x * TILE_SIZE;
                *y_pos = y * TILE_SIZE;
                map[y][x] = TILE_EMPTY;
                return;
            }
        }
    }
}


// Parses one patrol node line of the form:
//   { x , y } <list of next node indices>
// Indices are stored exactly as in the patrol_*.txt files (usually 1-based).
static void parse_node(const char* line, BotGraph* node, int id)
{
    node->id = id;
    node->x = 0;
    node->y = 0;
    node->next_index = 0;

    // Parse "{ x , y }"
    sscanf(line, " { %d , %d } ", &node->x, &node->y);

    const char* p = strchr(line, '}');
    if (!p) {
        return;
    }
    p++; // move past '}'

    // Parse all integers after the closing brace
    while (*p) {
        while (*p && isspace((unsigned char)*p)) {
            p++;
        }

        if (isdigit((unsigned char)*p)) {
            char* endp = NULL;
            long value = strtol(p, &endp, 10);
            if (endp != NULL && endp != p) {
                if (node->next_index < BOTGRAPH_MAX_LINKS) {
                    node->nextinds[node->next_index++] = (int)value;
                }
                p = endp;
                continue;
            }
        }

        // Skip any non-digit character
        p++;
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

    BotGraph* nodes = (BotGraph*)calloc((size_t)count, sizeof(BotGraph));
    if (!nodes) {
        fclose(f);
        return -1;
    }

    for (int i = 0; i < count; i++) {
        if (!fgets(buffer, sizeof(buffer), f)) {
            break;
        }
        parse_node(buffer, &nodes[i], i + 1);
    }

    fclose(f);

    *out_nodes = nodes;
    return count;
}

void free_graph(BotGraph* nodes, int count)
{
    (void)count;
    if (nodes == NULL) return;
    free(nodes);
}

