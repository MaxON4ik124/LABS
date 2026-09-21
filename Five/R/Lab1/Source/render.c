#include "main.h"

#include "stb_easy_font.h"

void draw_text(const char* text, float x, float y, float scale, float r, float g, float b) {
    if (!text || strlen(text) == 0) return;

    static char buffer[32768];
    int num_quads = stb_easy_font_print(x, y, (char*)text, NULL, buffer, sizeof(buffer));

    if (num_quads > 0) {
        glPushMatrix();
        glScalef(scale, scale, 1.0f);
        glColor3f(r, g, b);
        glEnableClientState(GL_VERTEX_ARRAY);
        glVertexPointer(2, GL_FLOAT, 16, buffer);
        glDrawArrays(GL_QUADS, 0, num_quads * 4);
        glDisableClientState(GL_VERTEX_ARRAY);
        glPopMatrix();
    }
}

void draw_map() {

    const float QUARTER = TILE_SIZE * 0.25f;
    const float HALF = TILE_SIZE * 0.5f;
    const float THREE_QUARTERS = TILE_SIZE * 0.75f;
    const float EIGHTH = TILE_SIZE * 0.125f;
    const float FIFTH = TILE_SIZE * 0.2f;


    glColor3f(0.2f, 0.2f, 0.2f);
    glBegin(GL_QUADS);
    for (int y = 0; y < MAP_HEIGHT; y++) {
        for (int x = 0; x < MAP_WIDTH; x++) {
            float x_pos = x * TILE_SIZE;
            float y_pos = y * TILE_SIZE;
            glVertex2f(x_pos, y_pos);
            glVertex2f(x_pos + TILE_SIZE, y_pos);
            glVertex2f(x_pos + TILE_SIZE, y_pos + TILE_SIZE);
            glVertex2f(x_pos, y_pos + TILE_SIZE);
        }
    }
    glEnd();

    glColor3f(0.25f, 0.25f, 0.25f);
    glBegin(GL_LINES);
    for (int y = 0; y < MAP_HEIGHT; y++) {
        for (int x = 0; x < MAP_WIDTH; x++) {
            if (texture_map[y][x] == 1) {
                float x_pos = x * TILE_SIZE;
                float y_pos = y * TILE_SIZE;
                glVertex2f(x_pos + QUARTER, y_pos + QUARTER);
                glVertex2f(x_pos + THREE_QUARTERS, y_pos + THREE_QUARTERS);
                glVertex2f(x_pos + THREE_QUARTERS, y_pos + QUARTER);
                glVertex2f(x_pos + QUARTER, y_pos + THREE_QUARTERS);
            }
        }
    }
    glEnd();

    glBegin(GL_LINES);
    for (int y = 0; y < MAP_HEIGHT; y++) {
        for (int x = 0; x < MAP_WIDTH; x++) {
            if (texture_map[y][x] == 2) {
                float x_pos = x * TILE_SIZE;
                float y_pos = y * TILE_SIZE;

                glVertex2f(x_pos + QUARTER, y_pos + QUARTER);
                glVertex2f(x_pos + THREE_QUARTERS, y_pos + QUARTER);

                glVertex2f(x_pos + THREE_QUARTERS, y_pos + QUARTER);
                glVertex2f(x_pos + THREE_QUARTERS, y_pos + THREE_QUARTERS);

                glVertex2f(x_pos + THREE_QUARTERS, y_pos + THREE_QUARTERS);
                glVertex2f(x_pos + QUARTER, y_pos + THREE_QUARTERS);

                glVertex2f(x_pos + QUARTER, y_pos + THREE_QUARTERS);
                glVertex2f(x_pos + QUARTER, y_pos + QUARTER);
            }
        }
    }
    glEnd();


    glBegin(GL_POINTS);
    for (int y = 0; y < MAP_HEIGHT; y++) {
        for (int x = 0; x < MAP_WIDTH; x++) {
            if (texture_map[y][x] == 3) {
                float x_pos = x * TILE_SIZE;
                float y_pos = y * TILE_SIZE;
                for (int i = 0; i < 5; i++) {
                    glVertex2f(x_pos + QUARTER + i * EIGHTH, y_pos + HALF);
                }
            }
        }
    }
    glEnd();


    glColor3f(0.5f, 0.5f, 0.5f);
    glBegin(GL_QUADS);
    for (int y = 0; y < MAP_HEIGHT; y++) {
        for (int x = 0; x < MAP_WIDTH; x++) {
            if (map[y][x] == TILE_WALL) {
                float x_pos = x * TILE_SIZE;
                float y_pos = y * TILE_SIZE;
                glVertex2f(x_pos, y_pos);
                glVertex2f(x_pos + TILE_SIZE, y_pos);
                glVertex2f(x_pos + TILE_SIZE, y_pos + TILE_SIZE);
                glVertex2f(x_pos, y_pos + TILE_SIZE);
            }
        }
    }
    glEnd();

    glColor3f(0.4f, 0.4f, 0.4f);
    glBegin(GL_LINES);
    for (int y = 0; y < MAP_HEIGHT; y++) {
        for (int x = 0; x < MAP_WIDTH; x++) {
            if (map[y][x] == TILE_WALL) {
                float x_pos = x * TILE_SIZE;
                float y_pos = y * TILE_SIZE;

                for (int i = 0; i < TILE_SIZE; i += TILE_SIZE / 4) {
                    glVertex2f(x_pos, y_pos + i);
                    glVertex2f(x_pos + TILE_SIZE, y_pos + i);
                }

                for (int i = 0; i < TILE_SIZE; i += TILE_SIZE / 4) {
                    glVertex2f(x_pos + i, y_pos);
                    glVertex2f(x_pos + i, y_pos + TILE_SIZE);
                }
            }
        }
    }
    glEnd();


    glColor3f(0.6f, 0.3f, 0.0f);
    glBegin(GL_QUADS);
    for (int y = 0; y < MAP_HEIGHT; y++) {
        for (int x = 0; x < MAP_WIDTH; x++) {
            if (map[y][x] == TILE_BREAKABLE) {
                float x_pos = x * TILE_SIZE;
                float y_pos = y * TILE_SIZE;
                glVertex2f(x_pos, y_pos);
                glVertex2f(x_pos + TILE_SIZE, y_pos);
                glVertex2f(x_pos + TILE_SIZE, y_pos + TILE_SIZE);
                glVertex2f(x_pos, y_pos + TILE_SIZE);
            }
        }
    }
    glEnd();


    glColor3f(0.5f, 0.25f, 0.0f);
    glBegin(GL_LINES);
    for (int y = 0; y < MAP_HEIGHT; y++) {
        for (int x = 0; x < MAP_WIDTH; x++) {
            if (map[y][x] == TILE_BREAKABLE) {
                float x_pos = x * TILE_SIZE;
                float y_pos = y * TILE_SIZE;

                glVertex2f(x_pos + QUARTER, y_pos + QUARTER);
                glVertex2f(x_pos + THREE_QUARTERS, y_pos + HALF);

                glVertex2f(x_pos + HALF, y_pos + QUARTER);
                glVertex2f(x_pos + QUARTER, y_pos + THREE_QUARTERS);

                glVertex2f(x_pos + THREE_QUARTERS, y_pos + QUARTER);
                glVertex2f(x_pos + HALF, y_pos + THREE_QUARTERS);
            }
        }
    }
    glEnd();


    glColor3f(0.0f, 0.3f, 0.7f);
    glBegin(GL_QUADS);
    for (int y = 0; y < MAP_HEIGHT; y++) {
        for (int x = 0; x < MAP_WIDTH; x++) {
            if (map[y][x] == TILE_WATER) {
                float x_pos = x * TILE_SIZE;
                float y_pos = y * TILE_SIZE;
                glVertex2f(x_pos, y_pos);
                glVertex2f(x_pos + TILE_SIZE, y_pos);
                glVertex2f(x_pos + TILE_SIZE, y_pos + TILE_SIZE);
                glVertex2f(x_pos, y_pos + TILE_SIZE);
            }
        }
    }
    glEnd();


    glColor3f(0.0f, 0.4f, 0.8f);
    glBegin(GL_LINES);
    for (int y = 0; y < MAP_HEIGHT; y++) {
        for (int x = 0; x < MAP_WIDTH; x++) {
            if (map[y][x] == TILE_WATER) {
                float x_pos = x * TILE_SIZE;
                float y_pos = y * TILE_SIZE;
                float water_offset = sinf((animation_time * 3.0f) + (x * 0.5f) + (y * 0.3f)) * 2.0f;

                for (int i = 0; i < TILE_SIZE; i += TILE_SIZE / 5) {
                    glVertex2f(x_pos, y_pos + i + water_offset);
                    glVertex2f(x_pos + TILE_SIZE, y_pos + i + water_offset);
                }
            }
        }
    }
    glEnd();


    glColor3f(0.8f, 0.9f, 1.0f);
    glBegin(GL_QUADS);
    for (int y = 0; y < MAP_HEIGHT; y++) {
        for (int x = 0; x < MAP_WIDTH; x++) {
            if (map[y][x] == TILE_ICE) {
                float x_pos = x * TILE_SIZE;
                float y_pos = y * TILE_SIZE;
                glVertex2f(x_pos, y_pos);
                glVertex2f(x_pos + TILE_SIZE, y_pos);
                glVertex2f(x_pos + TILE_SIZE, y_pos + TILE_SIZE);
                glVertex2f(x_pos, y_pos + TILE_SIZE);
            }
        }
    }
    glEnd();

    glColor3f(0.7f, 0.8f, 0.9f);
    glBegin(GL_LINES);
    for (int y = 0; y < MAP_HEIGHT; y++) {
        for (int x = 0; x < MAP_WIDTH; x++) {
            if (map[y][x] == TILE_ICE) {
                float x_pos = x * TILE_SIZE;
                float y_pos = y * TILE_SIZE;

                glVertex2f(x_pos, y_pos);
                glVertex2f(x_pos + TILE_SIZE, y_pos + TILE_SIZE);

                glVertex2f(x_pos + TILE_SIZE, y_pos);
                glVertex2f(x_pos, y_pos + TILE_SIZE);
            }
        }
    }
    glEnd();
}

void draw_ui() {
    glColor4f(0.0f, 0.0f, 0.0f, 0.7f);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glBegin(GL_QUADS);
    glVertex2f(0, 0);
    glVertex2f(WIDTH, 0);
    glVertex2f(WIDTH, 40);
    glVertex2f(0, 40);
    glEnd();

    glDisable(GL_BLEND);

    char health_text[50];
    sprintf(health_text, "Health: %d/%d", player.health, player.max_health);
    draw_text(health_text, 5, 2, 4.0f, 1.0f, 1.0f, 1.0f);

    char score_text[50];
    sprintf(score_text, "Score: %d", player.score);
    draw_text(score_text, 150, 2, 4.0f, 1.0f, 1.0f, 1.0f);

    char level_text[50];
    sprintf(level_text, "Level: %d/%d", level, MAX_LEVEL);
    draw_text(level_text, 250, 2, 4.0f, 1.0f, 1.0f, 1.0f);

    float powerup_x = 350;

    if (player.shield_timer > 0) {
        glColor3f(0.0f, 0.6f, 1.0f);
        draw_text("Shield", powerup_x, 2, 4.0f, 0.0f, 0.6f, 1.0f);
        powerup_x += 50;
    }

    if (player.rapid_fire_timer > 0) {
        draw_text("Rapidety", powerup_x, 2, 4.0f, 1.0f, 0.8f, 0.0f);
        powerup_x += 50;
    }

    if (player.triple_shot_timer > 0) {
        draw_text("Multiple shot", powerup_x, 2, 4.0f, 1.0f, 0.4f, 0.8f);
        powerup_x += 50;
    }
    if (player.speed_timer > 0) {
        draw_text("Speed", powerup_x, 2, 4.0f, 0.0f, 1.0f, 0.0f);
        powerup_x += 50;
    }

    if (message_timer > 0) {
        float alpha = (message_timer < 60) ? message_timer / 60.0f : 1.0f;
        glColor4f(1.0f, 1.0f, 1.0f, alpha);
        glEnable(GL_BLEND);

        float message_width = strlen(game_message) * 15.0f;
        draw_text(game_message, (WIDTH - message_width) / 2, HEIGHT - 50, 4.0f, 1.0f, 1.0f, 1.0f);

        glDisable(GL_BLEND);
    }
}

void draw_menu() {
    darkness_timer = 0;
    warning_active = false;
    darkness_active = false;
    glBegin(GL_QUADS);
    glColor3f(0.1f, 0.1f, 0.3f);
    glVertex2f(0, 0);
    glVertex2f(WIDTH, 0);
    glColor3f(0.2f, 0.2f, 0.4f);
    glVertex2f(WIDTH, HEIGHT);
    glVertex2f(0, HEIGHT);
    glEnd();

    glPointSize(2.0f);
    glColor3f(1.0f, 1.0f, 1.0f);
    glBegin(GL_POINTS);
    for (int i = 0; i < 200; i++) {
        float x = (sin(i * 3.14f + animation_time / 2) + 1) / 2 * WIDTH;
        float y = (cos(i * 2.71f + animation_time / 3) + 1) / 2 * HEIGHT;
        glVertex2f(x, y);
    }
    glEnd();

    float title_y = HEIGHT * 0.015f + sin(animation_time * 2) * 10;
    draw_text("2D TANKS", (WIDTH / 2 - 150) * 0.14f, title_y, 7.0f, 0.0f, 0.7f, 0.0f);

    float menu_y = HEIGHT * 0.05f;
    float menu_spacing = 30;

    if (menu_selection == 0) {
        glColor3f(0.0f, 1.0f, 0.0f);
        draw_text("> New Game <", (WIDTH / 2 - 25) * 0.13f, menu_y, 6.0f, 0.0f, 1.0f, 0.0f);
    }
    else {
        glColor3f(0.8f, 0.8f, 0.8f);
        draw_text("New Game", (WIDTH / 2 + 75) * 0.13f, menu_y, 6.0f, 0.8f, 0.8f, 0.8f);
    }

    menu_y += menu_spacing;
    if (menu_selection == 1) {
        char level_text[150];
        sprintf(level_text, "> Level: %d - %s <", level, level_info[level].name);
        draw_text(level_text, (WIDTH / 2 - 250) * 0.13f, menu_y, 6.0f, 0.0f, 1.0f, 0.0f);
    }
    else {
        char level_text[150];
        sprintf(level_text, "Level: %d - %s", level, level_info[level].name);
        draw_text(level_text, (WIDTH / 2 - 150) * 0.13f, menu_y, 6.0f, 0.8f, 0.8f, 0.8f);
    }

    menu_y += menu_spacing;
    if (menu_selection == 2) {
        draw_text("> Exit <", (WIDTH / 2 + 100) * 0.13f, menu_y, 6.0f, 0.0f, 1.0f, 0.0f);
    }
    else {
        draw_text("Exit", (WIDTH / 2 + 200) * 0.13f, menu_y, 6.0f, 0.8f, 0.8f, 0.8f);
    }
    menu_y += 3 * menu_spacing;
    glColor3f(0.7f, 0.7f, 0.7f);
    draw_text("Controls: WASD - movement, SAPCE - shoot, QE - tower rotation", (WIDTH / 2 - 350) * 0.13f, menu_y, 4.0f, 0.7f, 0.7f, 0.7f);
    menu_y += menu_spacing / 2;
    draw_text("P - pause, ESC - main menu", (WIDTH / 2 + 300) * 0.13f, menu_y, 4.0f, 0.7f, 0.7f, 0.7f);
}

void draw_win_screen() {
    glBegin(GL_QUADS);
    glColor3f(0.0f, 0.2f, 0.0f);
    glVertex2f(0, 0);
    glVertex2f(WIDTH, 0);
    glColor3f(0.0f, 0.5f, 0.0f);
    glVertex2f(WIDTH, HEIGHT);
    glVertex2f(0, HEIGHT);
    glEnd();

    glPointSize(3.0f);
    glColor3f(1.0f, 1.0f, 0.3f);
    glBegin(GL_POINTS);
    for (int i = 0; i < 300; i++) {
        float x = (sin(i * 1.23f + animation_time) + 1) / 2 * WIDTH;
        float y = (cos(i * 2.34f + animation_time * 1.5f) + 1) / 2 * HEIGHT;
        glVertex2f(x, y);
    }
    glEnd();

    float scale = 9.0f + sin(animation_time * 3) * 0.2f;
    glColor3f(0.0f, 1.0f, 0.0f);
    draw_text("VICTORY!", (WIDTH / 2 - 280) * 0.13f, (HEIGHT / 2 + 150) * 0.015f, scale, 0.0f, 1.0f, 0.0f);

    char score_text[50];
    sprintf(score_text, "Total score: %d", player.score);
    glColor3f(1.0f, 1.0f, 1.0f);
    draw_text(score_text, (WIDTH / 2 - 100) * 0.13f, (HEIGHT / 2 + 200) * 0.15f, 6.0f, 1.0f, 1.0f, 1.0f);

    glColor3f(0.9f, 0.9f, 0.0f);
    draw_text("Congratulations! You completed all levels!", (WIDTH / 2 - 520) * 0.13f, (HEIGHT / 2) * 0.15f, 6.0f, 0.9f, 0.9f, 0.0f);

    if ((int)(animation_time * 2) % 2 == 0) {
        glColor3f(0.8f, 0.8f, 0.8f);
        draw_text("Press ENTER to exit in main menu", (WIDTH / 2 - 500) * 0.13f, (HEIGHT / 2 + 400) * 0.15f, 6.0f, 0.8f, 0.8f, 0.8f);
    }
}

void draw_pause_menu() {
    glColor4f(0.0f, 0.0f, 0.0f, 0.6f);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glBegin(GL_QUADS);
    glVertex2f(0, 0);
    glVertex2f(WIDTH, 0);
    glVertex2f(WIDTH, HEIGHT);
    glVertex2f(0, HEIGHT);
    glEnd();

    float scale = 8.0f + sin(animation_time * 2) * 0.1f;
    glColor3f(1.0f, 1.0f, 1.0f);
    draw_text("PAUSE", (WIDTH / 2 - 200) * 0.13f, (HEIGHT / 2 - 450) * 0.15f, scale, 1.0f, 1.0f, 1.0f);

    glColor3f(0.8f, 0.8f, 0.8f);
    draw_text("Press P for continue", (WIDTH / 2 - 150) * 0.13f, (HEIGHT / 2) * 0.15f, 6.0f, 0.8f, 0.8f, 0.8f);
    draw_text("Press ESC for exit in main menu", (WIDTH / 2 - 380) * 0.13f, (HEIGHT / 2 + 300) * 0.15f, 6.0f, 0.8f, 0.8f, 0.8f);

    glDisable(GL_BLEND);
}

void draw_game_over() {
    glBegin(GL_QUADS);
    glColor3f(0.2f, 0.0f, 0.0f);
    glVertex2f(0, 0);
    glVertex2f(WIDTH, 0);
    glColor3f(0.5f, 0.0f, 0.0f);
    glVertex2f(WIDTH, HEIGHT);
    glVertex2f(0, HEIGHT);
    glEnd();

    float y_offset = sin(animation_time * 2) * 20.0f;
    glColor4f(1.0f, 0.0f, 0.0f, 1.0f);
    draw_text("GAME OVER", (WIDTH / 2 - 250) * 0.13f, (HEIGHT / 2 - 450 + y_offset) * 0.15f, 8.0f, 1.0f, 0.0f, 0.0f);

    char score_text[50];
    sprintf(score_text, "Your Score: %d", player.score);
    glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
    draw_text(score_text, (WIDTH / 2) * 0.13f, (HEIGHT / 2) * 0.15f, 6.0f, 1.0f, 1.0f, 1.0f);

    if ((int)(animation_time * 2) % 2 == 0) {
        glColor4f(0.8f, 0.8f, 0.8f, 1.0f);
        draw_text("Press ENTER to return to main menu", (WIDTH / 2 - 450) * 0.13f, (HEIGHT / 2 + 500) * 0.15f, 6.0f, 0.8f, 0.8f, 0.8f);
    }

    glDisable(GL_BLEND);
}

void render() {
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

    glClear(GL_COLOR_BUFFER_BIT);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0, WIDTH, HEIGHT, 0, -1, 1);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    if (game_state == GAME_MENU) {
        draw_menu();
    }

    if (game_state == GAME_PLAYING) {
        draw_map();

        for (int i = 0; i < MAX_POWERUPS; i++) {
            if (powerups[i].active) {
                draw_powerup(powerups[i]);
            }
        }

        for (int i = 0; i < MAX_BULLETS * (MAX_BOTS + 1); i++) {
            if (bullets[i].active) {
                draw_bullet(bullets[i]);
            }
        }

        if (player.active) {
            draw_tank(player, true);
        }

        for (int i = 0; i < MAX_BOTS; i++) {
            if (bots[i].active) {
                draw_tank(bots[i], false);
            }
        }

        draw_particles();
        render_darkness_overlay();
        render_warning_pulse();
        draw_ui();
    }

    if (game_state == GAME_LEVEL_TRANSITION)
        draw_level_transition();

    if (game_state == GAME_PAUSED) {
        draw_map();

        for (int i = 0; i < MAX_POWERUPS; i++) {
            if (powerups[i].active) {
                draw_powerup(powerups[i]);
            }
        }

        for (int i = 0; i < MAX_BULLETS * (MAX_BOTS + 1); i++) {
            if (bullets[i].active) {
                draw_bullet(bullets[i]);
            }
        }

        if (player.active) {
            draw_tank(player, true);
        }

        for (int i = 0; i < MAX_BOTS; i++) {
            if (bots[i].active) {
                draw_tank(bots[i], false);
            }
        }

        draw_particles();
        render_warning_pulse();
        draw_ui();
        draw_pause_menu();
    }

    if (game_state == GAME_OVER)
        draw_game_over();

    if (game_state == GAME_WIN)
        draw_win_screen();
}

void draw_level_transition() {
    float alpha = 0.0f;

    if (transition_fade_out) {
        alpha = transition_timer / FADE_OUT_TIME;
        if (alpha > 1.0f) alpha = 1.0f;
    }
    else {
        alpha = 1.0f - (transition_timer / FADE_IN_TIME);
        if (alpha < 0.0f) alpha = 0.0f;
    }

    if (!transition_fade_out || alpha < 1.0f) {
        draw_map();

        for (int i = 0; i < MAX_POWERUPS; i++) {
            if (powerups[i].active) {
                draw_powerup(powerups[i]);
            }
        }

        for (int i = 0; i < MAX_BULLETS * (MAX_BOTS + 1); i++) {
            if (bullets[i].active) {
                draw_bullet(bullets[i]);
            }
        }

        if (player.active) {
            draw_tank(player, true);
        }

        for (int i = 0; i < MAX_BOTS; i++) {
            if (bots[i].active) {
                draw_tank(bots[i], false);
            }
        }

        draw_particles();
        draw_ui();
    }

    glColor4f(0.0f, 0.0f, 0.0f, alpha);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glBegin(GL_QUADS);
    glVertex2f(0, 0);
    glVertex2f(WIDTH, 0);
    glVertex2f(WIDTH, HEIGHT);
    glVertex2f(0, HEIGHT);
    glEnd();

    if (transition_fade_out && alpha > 0.5f) {
        float text_alpha = (alpha - 0.5f) * 2.0f;

        glColor4f(1.0f, 1.0f, 1.0f, text_alpha);

        char transition_text[100];
        if (next_level <= MAX_LEVEL) {
            sprintf(transition_text, "Level %d", next_level);
            draw_text(transition_text, (WIDTH / 2 + 350) * 0.13f, (HEIGHT / 2 - 350) * 0.15f, 5.0f, 1.0f, 1.0f, 1.0f);

            sprintf(transition_text, "%s", level_info[next_level].name);
            draw_text(transition_text, (WIDTH / 2 + 275) * 0.13f, (HEIGHT / 2) * 0.15f, 5.0f, 0.8f, 0.8f, 0.8f);

            draw_text(level_info[next_level].description, (WIDTH / 2) * 0.13f, (HEIGHT / 2 + 550) * 0.15f, 4.0f, 0.6f, 0.6f, 0.6f);

            if (next_level == LEVEL_TOWN_UP)
                draw_text("ATTENTION! After 15 secons lights will be put off!", (WIDTH / 2 - 450) * 0.13f, (HEIGHT / 2 + 670) * 0.15f, 5.0f, 1.0f, 0.0f, 0.0f);
        }
    }

    if (alpha > 0.3f) {
        glPointSize(2.0f + sin(animation_time * 4) * 0.5f);
        glColor4f(0.3f, 0.6f, 1.0f, alpha * 0.7f);
        glBegin(GL_POINTS);
        for (int i = 0; i < 100; i++) {
            float x = (sin(i * 1.23f + animation_time * 2) + 1) / 2 * WIDTH;
            float y = (cos(i * 2.34f + animation_time * 1.5f) + 1) / 2 * HEIGHT;
            glVertex2f(x, y);
        }
        glEnd();
    }

    glDisable(GL_BLEND);
}