#include "main.h"

void update_powerups(float dt) {
    serial();
    for (int i = 0; i < MAX_POWERUPS; i++) {
        if (powerups[i].active) {
            powerups[i].timer--;

            if (powerups[i].timer <= 0) {
                powerups[i].active = false;
                continue;
            }

            powerups[i].rotation += 90.0f * dt;
            if (powerups[i].rotation > 360.0f) {
                powerups[i].rotation -= 360.0f;
            }

            if (powerups[i].scaling_up) {
                powerups[i].scale += 0.5f * dt;
                if (powerups[i].scale >= 1.2f) {
                    powerups[i].scaling_up = false;
                }
            }
            else {
                powerups[i].scale -= 0.5f * dt;
                if (powerups[i].scale <= 0.8f) {
                    powerups[i].scaling_up = true;
                }
            }
        }
    }
}

void spawn_powerup() {
    for (int i = 0; i < MAX_POWERUPS; i++) {
        if (!powerups[i].active) {
            float x, y;
            bool valid_position = false;
            int attempts = 0;

            const float min_dist = TANK_SIZE * 2.0f;
            const float min_dist2 = min_dist * min_dist;

            while (!valid_position && attempts < 50) {
                int tile_x = rand() % (MAP_WIDTH - 2) + 1;
                int tile_y = rand() % (MAP_HEIGHT - 2) + 1;

                if (map[tile_y][tile_x] == TILE_EMPTY) {
                    x = tile_x * TILE_SIZE + TILE_SIZE / 2;
                    y = tile_y * TILE_SIZE + TILE_SIZE / 2;

                    bool too_close = false;

                    if (player.active && dist2f(x, y, player.x, player.y) < min_dist2) {
                        too_close = true;
                    }

                    for (int j = 0; j < MAX_BOTS; j++) {
                        if (bots[j].active && dist2f(x, y, bots[j].x, bots[j].y) < min_dist2) {
                            too_close = true;
                            break;
                        }
                    }

                    if (!too_close) {
                        valid_position = true;
                    }
                }

                attempts++;
            }

            if (valid_position) {
                powerups[i].x = x;
                powerups[i].y = y;
                powerups[i].type = rand() % POWERUP_COUNT;
                powerups[i].active = true;
                powerups[i].timer = 3600;
                powerups[i].rotation = 0;
                powerups[i].scale = 1.0f;
                powerups[i].scaling_up = false;

                add_particles(
                    powerups[i].x, powerups[i].y,
                    powerup_colors[powerups[i].type][0],
                    powerup_colors[powerups[i].type][1],
                    powerup_colors[powerups[i].type][2],
                    15,
                    60.0f,
                    2.5f,
                    0.6f
                );

                break;
            }
        }
    }
}

void apply_powerup(PowerupType type) {
    switch (type) {
    case POWERUP_HEALTH:
        player.health = fmin(player.max_health, player.health + BASE_TANK_HP * 0.2);
        break;

    case POWERUP_SPEED:
        player.speed_timer = 1200;
        for (int i = 0; i < MAX_BULLETS * (MAX_BOTS + 1); i++) {
            if (!bullets[i].active) {
                bullets[i].active = true;
                bullets[i].is_player = true;
                bullets[i].x = player.x;
                bullets[i].y = player.y;
                bullets[i].dx = 0;
                bullets[i].dy = 0;
                bullets[i].damage = 0;
                bullets[i].life_time = 10.0f;
                break;
            }
        }
        break;

    case POWERUP_RAPID_FIRE:
        player.rapid_fire_timer = 1200;
        break;

    case POWERUP_SHIELD:
        player.shield_timer = 1200;
        break;

    case POWERUP_TRIPLE_SHOT:
        player.triple_shot_timer = 1200;
        break;
    }

    message_timer = 1200;
}

void draw_powerup(Powerup powerup) {
    if (!powerup.active) return;

    glPushMatrix();
    glTranslatef(powerup.x, powerup.y, 0);
    glRotatef(powerup.rotation, 0, 0, 1);
    glScalef(powerup.scale, powerup.scale, 1.0f);

    float* color = powerup_colors[powerup.type];
    glColor3f(color[0], color[1], color[2]);

    glBegin(GL_TRIANGLE_FAN);
    glVertex2f(0, 0);
    for (int i = 0; i <= 360; i += 20) {
        float radian = i * M_PI / 180.0f;
        glVertex2f(cos(radian) * POWERUP_SIZE / 2, sin(radian) * POWERUP_SIZE / 2);
    }
    glEnd();

    glColor3f(1.0f, 1.0f, 1.0f);
    switch (powerup.type) {
    case POWERUP_HEALTH:
        glBegin(GL_QUADS);
        glVertex2f(-2, -10);
        glVertex2f(2, -10);
        glVertex2f(2, 10);
        glVertex2f(-2, 10);
        glVertex2f(-10, -2);
        glVertex2f(10, -2);
        glVertex2f(10, 2);
        glVertex2f(-10, 2);
        glEnd();
        break;

    case POWERUP_SPEED:
        glBegin(GL_TRIANGLES);
        glVertex2f(-5, 10);
        glVertex2f(0, 0);
        glVertex2f(5, 0);
        glVertex2f(0, 0);
        glVertex2f(5, 0);
        glVertex2f(0, -10);
        glEnd();
        break;

    case POWERUP_RAPID_FIRE:
        glBegin(GL_TRIANGLES);
        glVertex2f(-8, 0);
        glVertex2f(0, -8);
        glVertex2f(0, 8);
        glVertex2f(0, -8);
        glVertex2f(0, 8);
        glVertex2f(8, 0);
        glEnd();
        break;

    case POWERUP_SHIELD:
        glBegin(GL_TRIANGLE_FAN);
        glVertex2f(0, 0);
        for (int i = -90; i <= 90; i += 15) {
            float radian = i * M_PI / 180.0f;
            glVertex2f(cos(radian) * POWERUP_SIZE / 3, sin(radian) * POWERUP_SIZE / 3);
        }
        glEnd();
        break;

    case POWERUP_TRIPLE_SHOT:
        glBegin(GL_LINES);
        glVertex2f(-8, -8);
        glVertex2f(-4, 8);
        glVertex2f(0, -10);
        glVertex2f(0, 10);
        glVertex2f(8, -8);
        glVertex2f(4, 8);
        glEnd();
        break;
    }

    glPopMatrix();
}