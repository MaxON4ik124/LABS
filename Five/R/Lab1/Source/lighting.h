#ifndef LIGHTING_H
#define LIGHTING_H

#include "main.h"


#define FOV_ANGLE 60.0f          
#define MAX_VISIBILITY_RAYS 360 
#define WARNING_DURATION 180
#define WARNING_PULSES 3
#define DARKNESS_START_TIME 900


typedef struct {
    float x, y;
    float distance;
    uint8_t blocked;
} VisibilityPoint;

extern bool darkness_active;
extern bool warning_active;
extern int darkness_timer;
extern int warning_timer;
extern int pulse_count;
extern float pulse_intensity;
extern VisibilityPoint visibility_polygon[MAX_VISIBILITY_RAYS];
extern int visibility_point_count;

void init_lighting(void);
void update_lighting(float dt);
void calculate_visibility(float player_x, float player_y, float player_angle);
bool is_point_visible(float x, float y, float player_x, float player_y, float player_angle);

void render_darkness_overlay(void);
void render_warning_pulse(void);

#endif