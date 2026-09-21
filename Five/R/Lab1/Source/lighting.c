#include "main.h"


bool darkness_active = false;
bool warning_active = false;
int darkness_timer = 0;
int warning_timer = 0;
int pulse_count = 0;
float pulse_intensity = 0.0f;
VisibilityPoint visibility_polygon[MAX_VISIBILITY_RAYS];
int visibility_point_count = 0;

void init_lighting(void) {
    darkness_active = false;
    warning_active = false;
    darkness_timer = 0;
    warning_timer = 0;
    pulse_count = 0;
    pulse_intensity = 0.0f;
    visibility_point_count = 0;
}

void update_lighting(float dt) {
    serial();
    if (level < 3 || game_state != GAME_PLAYING) {
        darkness_active = false;
        warning_active = false;
        return;
    }

    darkness_timer++;


    if (darkness_timer >= DARKNESS_START_TIME && !warning_active && !darkness_active) {
        warning_active = true;
        warning_timer = 0;
        pulse_count = 0;
    }


    if (warning_active) {
        warning_timer++;


        float pulse_cycle = warning_timer % (WARNING_DURATION / WARNING_PULSES);
        float pulse_progress = pulse_cycle / (float)(WARNING_DURATION / WARNING_PULSES);

        if (pulse_progress <= 0.5f) {
            pulse_intensity = pulse_progress * 2.0f;
        }
        else {
            pulse_intensity = 2.0f - pulse_progress * 2.0f;
        }


        if (warning_timer >= WARNING_DURATION) {
            warning_active = false;
            darkness_active = true;
            pulse_intensity = 0.0f;
        }
    }


    if (darkness_active) {
        calculate_visibility(player.x, player.y, player.angle);
    }
}


bool ray_intersects_wall(float x1, float y1, float x2, float y2, float* hit_x, float* hit_y, float* distance) {
    float dx = x2 - x1;
    float dy = y2 - y1;
    float length = sqrtf(dx * dx + dy * dy);

    if (length == 0) return false;

    dx /= length;
    dy /= length;

    float step = TILE_SIZE / 45.0f;
    float current_x = x1;
    float current_y = y1;
    float traveled = 0;

    while (traveled < length) {
        int tile_x = (int)(current_x / TILE_SIZE);
        int tile_y = (int)(current_y / TILE_SIZE);


        if (tile_x < 0 || tile_x >= MAP_WIDTH || tile_y < 0 || tile_y >= MAP_HEIGHT) {
            *hit_x = current_x;
            *hit_y = current_y;
            *distance = traveled;
            return true;
        }


        if (map[tile_y][tile_x] == TILE_WALL || map[tile_y][tile_x] == TILE_BREAKABLE) {
            *hit_x = current_x;
            *hit_y = current_y;
            *distance = traveled;
            return true;
        }

        current_x += dx * step;
        current_y += dy * step;
        traveled += step;
    }

    *hit_x = x2;
    *hit_y = y2;
    *distance = length;
    return false;
}


void calculate_visibility(float player_x, float player_y, float player_angle) {
    visibility_point_count = 0;

    float half_fov = FOV_ANGLE / 2.0f;
    float start_angle = player_angle - half_fov;
    float end_angle = player_angle + half_fov;


    while (start_angle < 0) start_angle += 360.0f;
    while (end_angle >= 360.0f) end_angle -= 360.0f;


    int rays_in_fov = (int)(FOV_ANGLE * 5);

    for (int i = 0; i <= rays_in_fov; i++) {
        float angle = start_angle + (FOV_ANGLE * i / (float)rays_in_fov);

        // �������
        while (angle < 0) angle += 360.0f;
        while (angle >= 360.0f) angle -= 360.0f;

        float rad = angle * M_PI / 180.0f;

        // ������������ ��������� ���� (��������� ������)
        float max_distance = sqrtf(WIDTH * WIDTH + HEIGHT * HEIGHT);

        float end_x = player_x + cosf(rad) * max_distance;
        float end_y = player_y + sinf(rad) * max_distance;

        float hit_x, hit_y, distance;
        bool hit = ray_intersects_wall(player_x, player_y, end_x, end_y, &hit_x, &hit_y, &distance);

        if (visibility_point_count < MAX_VISIBILITY_RAYS) {
            visibility_polygon[visibility_point_count].x = hit_x;
            visibility_polygon[visibility_point_count].y = hit_y;
            visibility_polygon[visibility_point_count].distance = distance;
            visibility_polygon[visibility_point_count].blocked = hit;
            visibility_point_count++;
        }
    }
}


bool is_point_visible(float x, float y, float player_x, float player_y, float player_angle) {
    if (!darkness_active) return true;

    float dx = x - player_x;
    float dy = y - player_y;
    float point_angle = atan2f(dy, dx) * 180.0f / M_PI;


    while (point_angle < 0) point_angle += 360.0f;
    while (point_angle >= 360.0f) point_angle -= 360.0f;


    float half_fov = FOV_ANGLE / 2.0f;
    float angle_diff = fabs(point_angle - player_angle);


    if (angle_diff > 180.0f) {
        angle_diff = 360.0f - angle_diff;
    }

    if (angle_diff > half_fov) return false;

    float hit_x, hit_y, distance;
    float point_distance = sqrtf(dx * dx + dy * dy);

    bool blocked = ray_intersects_wall(player_x, player_y, x, y, &hit_x, &hit_y, &distance);

    return !blocked || distance >= point_distance;
}



void render_darkness_overlay(void) {
    if (!darkness_active) return;


    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);


    glEnable(GL_STENCIL_TEST);


    glStencilMask(0xFF);
    glClear(GL_STENCIL_BUFFER_BIT);


    glStencilFunc(GL_ALWAYS, 1, 0xFF);
    glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);
    glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);


    glBegin(GL_TRIANGLE_FAN);
    glVertex2f(player.x, player.y);
    for (int i = 0; i < visibility_point_count; i++) {
        glVertex2f(visibility_polygon[i].x, visibility_polygon[i].y);
    }
    if (visibility_point_count > 0) {
        glVertex2f(visibility_polygon[0].x, visibility_polygon[0].y);
    }
    glEnd();

    const float player_light_radius = 60.0f;
    glStencilFunc(GL_ALWAYS, 1, 0xFF);
    glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);


    glBegin(GL_TRIANGLE_FAN);
    glVertex2f(player.x, player.y);
    for (int i = 0; i <= 360; i += 10) {
        float angle = i * M_PI / 180.0f;
        glVertex2f(player.x + cosf(angle) * player_light_radius,
            player.y + sinf(angle) * player_light_radius);
    }
    glEnd();


    glStencilFunc(GL_NOTEQUAL, 1, 0xFF);
    glStencilOp(GL_KEEP, GL_KEEP, GL_KEEP);
    glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);


    glColor4f(0.0f, 0.0f, 0.0f, 0.65f);
    glBegin(GL_QUADS);
    glVertex2f(0, 40);
    glVertex2f(WIDTH, 40);
    glVertex2f(WIDTH, HEIGHT);
    glVertex2f(0, HEIGHT);
    glEnd();


    glDisable(GL_STENCIL_TEST);
    glDisable(GL_BLEND);
}

void render_warning_pulse(void) {
    if (!warning_active) return;

    glColor4f(1.0f, 0.0f, 0.0f, pulse_intensity * 0.5f);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glBegin(GL_QUADS);
    glVertex2f(0, 0);
    glVertex2f(WIDTH, 0);
    glVertex2f(WIDTH, HEIGHT);
    glVertex2f(0, HEIGHT);
    glEnd();

    glDisable(GL_BLEND);
}