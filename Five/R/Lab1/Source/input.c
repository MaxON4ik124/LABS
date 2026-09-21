#include "main.h"

void process_input(GLFWwindow* window) {
    if (game_state == GAME_PLAYING) {
        float speed = player.speed * delta_time;
        float dx = 0.0f, dy = 0.0f;
        bool movement_input = false;

        if (keys[GLFW_KEY_W]) {
            dy -= speed;
            movement_input = true;
        }
        if (keys[GLFW_KEY_S]) {
            dy += speed;
            movement_input = true;
        }
        if (keys[GLFW_KEY_A]) {
            dx -= speed;
            movement_input = true;
        }
        if (keys[GLFW_KEY_D]) {
            dx += speed;
            movement_input = true;
        }

        if (dx != 0.0f || dy != 0.0f) {
            player.movement_angle = atan2f(dy, dx) * 180.0f / M_PI;

            if (dx != 0.0f && dy != 0.0f) {
                float length = sqrtf(dx * dx + dy * dy);
                dx = dx / length * speed;
                dy = dy / length * speed;
            }

            float new_x = player.x + dx;
            float new_y = player.y + dy;

            if (!check_map_collision(new_x, player.y, TANK_SIZE / 2)) {
                player.x = new_x;
            }
            if (!check_map_collision(player.x, new_y, TANK_SIZE / 2)) {
                player.y = new_y;
            }
        }

        if (keys[GLFW_KEY_Q]) {
            player.target_angle -= player.rotation_speed * delta_time;
        }
        if (keys[GLFW_KEY_E]) {
            player.target_angle += player.rotation_speed * delta_time;
        }

        float angle_diff = player.target_angle - player.angle;
        while (angle_diff > 180.0f) angle_diff -= 360.0f;
        while (angle_diff < -180.0f) angle_diff += 360.0f;

        if (fabsf(angle_diff) > 0.1f) {
            float rotation_amount = player.rotation_speed * delta_time;
            if (fabsf(angle_diff) < rotation_amount) {
                player.angle = player.target_angle;
            }
            else if (angle_diff > 0) {
                player.angle += rotation_amount;
            }
            else {
                player.angle -= rotation_amount;
            }

            while (player.angle >= 360.0f) player.angle -= 360.0f;
            while (player.angle < 0.0f) player.angle += 360.0f;
        }

        if (keys[GLFW_KEY_SPACE] && player.cooldown <= 0) {
            player.shooting = true;

            int shots = 1;
            float angle_offset = 0;

            if (player.triple_shot_timer > 0) {
                shots = 3;
                angle_offset = 15.0f;
            }

            for (int i = 0; i < shots; i++) {
                float shot_angle = player.angle;
                if (shots > 1) shot_angle += angle_offset * (i - 1);

                float rad = shot_angle * M_PI / 180.0f;

                for (int j = 0; j < MAX_BULLETS; j++) {
                    if (!bullets[j].active) {
                        bullets[j].x = player.x + cosf(rad) * TANK_SIZE;
                        bullets[j].y = player.y + sinf(rad) * TANK_SIZE;
                        bullets[j].dx = cosf(rad) * BULLET_SPEED;
                        bullets[j].dy = sinf(rad) * BULLET_SPEED;
                        bullets[j].active = true;
                        bullets[j].is_player = true;
                        bullets[j].damage = 25;
                        bullets[j].life_time = 3.0f;

                        add_particles(
                            bullets[j].x, bullets[j].y,
                            1.0f, 0.8f, 0.0f,
                            10,
                            50.0f,
                            3.0f,
                            0.5f
                        );

                        break;
                    }
                }
            }

            player.cooldown = (player.rapid_fire_timer > 0)
                ? RELOAD_TIME / 2
                : RELOAD_TIME;
        }
        else {
            player.shooting = false;
        }
    }
    else if (game_state == GAME_MENU) {
        if (keys[GLFW_KEY_UP] && !keys[GLFW_KEY_UP + 1000]) {
            keys[GLFW_KEY_UP + 1000] = true;
            if (menu_selection == 0) menu_selection = 2;
            else menu_selection--;
        }

        if (keys[GLFW_KEY_DOWN] && !keys[GLFW_KEY_DOWN + 1000]) {
            keys[GLFW_KEY_DOWN + 1000] = true;
            if (menu_selection == 2) menu_selection = 0;
            else menu_selection++;
        }
        if (keys[GLFW_KEY_ENTER] && !keys[GLFW_KEY_ENTER + 1000]) {
            keys[GLFW_KEY_ENTER + 1000] = true;

            switch (menu_selection) {
            case 0:
                init_level(level);
                game_state = GAME_PLAYING;
                break;
            case 1:
                level = (level % MAX_LEVEL) + 1;
                break;
            case 2:
                glfwSetWindowShouldClose(glfwGetCurrentContext(), GLFW_TRUE);
                break;
            }
        }
    }
    else if (game_state == GAME_PAUSED) {
        if (keys[GLFW_KEY_P] && !keys[GLFW_KEY_P + 1000]) {
            keys[GLFW_KEY_P + 1000] = true;
            game_state = GAME_PLAYING;
        }

        if (keys[GLFW_KEY_ESCAPE] && !keys[GLFW_KEY_ESCAPE + 1000]) {
            keys[GLFW_KEY_ESCAPE + 1000] = true;
            darkness_timer = 0;
            warning_active = false;
            darkness_active = false;
            game_state = GAME_MENU;
        }
    }
    else if (game_state == GAME_OVER || game_state == GAME_WIN) {
        if (keys[GLFW_KEY_ENTER] && !keys[GLFW_KEY_ENTER + 1000]) {
            keys[GLFW_KEY_ENTER + 1000] = true;
            game_state = GAME_MENU;
        }
    }

    if (!keys[GLFW_KEY_UP]) keys[GLFW_KEY_UP + 1000] = false;
    if (!keys[GLFW_KEY_DOWN]) keys[GLFW_KEY_DOWN + 1000] = false;
    if (!keys[GLFW_KEY_ENTER]) keys[GLFW_KEY_ENTER + 1000] = false;
    if (!keys[GLFW_KEY_P]) keys[GLFW_KEY_P + 1000] = false;
    if (!keys[GLFW_KEY_ESCAPE]) keys[GLFW_KEY_ESCAPE + 1000] = false;
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    if (key == GLFW_KEY_W) {
        keys[GLFW_KEY_W] = (action != GLFW_RELEASE);
    }
    if (key == GLFW_KEY_S) {
        keys[GLFW_KEY_S] = (action != GLFW_RELEASE);
    }
    if (key == GLFW_KEY_A) {
        keys[GLFW_KEY_A] = (action != GLFW_RELEASE);
    }
    if (key == GLFW_KEY_D) {
        keys[GLFW_KEY_D] = (action != GLFW_RELEASE);
    }
    if (key == GLFW_KEY_Q) {
        keys[GLFW_KEY_Q] = (action != GLFW_RELEASE);
    }
    if (key == GLFW_KEY_E) {
        keys[GLFW_KEY_E] = (action != GLFW_RELEASE);
    }
    if (key == GLFW_KEY_SPACE) {
        keys[GLFW_KEY_SPACE] = (action != GLFW_RELEASE);
    }

    if (key == GLFW_KEY_P && action == GLFW_PRESS) {
        keys[GLFW_KEY_P] = true;
        if (game_state == GAME_PLAYING) {
            game_state = GAME_PAUSED;
        }
        else if (game_state == GAME_PAUSED) {
            game_state = GAME_PLAYING;
        }
    }

    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
        keys[GLFW_KEY_ESCAPE] = true;
        if (game_state == GAME_PLAYING || game_state == GAME_PAUSED) {
            game_state = GAME_MENU;
        }
    }

    if (key == GLFW_KEY_UP && action == GLFW_PRESS) {
        keys[GLFW_KEY_UP] = true;
    }
    if (key == GLFW_KEY_DOWN && action == GLFW_PRESS) {
        keys[GLFW_KEY_DOWN] = true;
    }
    if (key == GLFW_KEY_ENTER && action == GLFW_PRESS) {
        keys[GLFW_KEY_ENTER] = true;
    }
    if (key == GLFW_KEY_UP && action == GLFW_RELEASE) {
        keys[GLFW_KEY_UP] = false;
    }
    if (key == GLFW_KEY_DOWN && action == GLFW_RELEASE) {
        keys[GLFW_KEY_DOWN] = false;
    }
    if (key == GLFW_KEY_ENTER && action == GLFW_RELEASE) {
        keys[GLFW_KEY_ENTER] = false;
    }
}