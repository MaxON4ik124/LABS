#ifndef POWERUP_H
#define POWERUP_H

#include "main.h"


void update_powerups(float dt);
void spawn_powerup(void);
void apply_powerup(PowerupType type);
void draw_powerup(Powerup powerup);

#endif
