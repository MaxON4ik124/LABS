#ifndef GAME_H
#define GAME_H

#include "main.h"

void init_game(void);
void init_level(int level);
void update_game(float dt);
void update_level_transition(float dt);
#endif