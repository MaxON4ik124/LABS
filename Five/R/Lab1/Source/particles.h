#ifndef PARTICLES_H
#define PARTICLES_H

#include "main.h"


void update_particles(float dt);
void add_particles(float x, float y, float r, float g, float b, int count, float speed, float size, float lifetime);
void create_explosion(float x, float y, float r, float g, float b, int count, float size);
void draw_particles(void);

#endif 
