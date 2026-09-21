#ifndef TANK_H
#define TANK_H

#include "main.h"

void update_bots(float dt);
void update_bullets(float dt);
void check_collisions(void);
bool check_tank_collision(float x, float y, Tank* exclude_tank);
float distance(float x1, float y1, float x2, float y2);
void draw_tank(Tank tank, bool is_player);
void draw_bullet(Bullet bullet);
int select_next_node_for_level2(Tank* bot);
#endif 
