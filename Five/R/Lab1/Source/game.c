#include "main.h"

Tank player;
Tank bots[MAX_BOTS];
Bullet bullets[MAX_BULLETS * (MAX_BOTS + 1)];
Powerup powerups[MAX_POWERUPS];
bool keys[2048] = { 0 };
double last_time = 0;
double delta_time = 0;
GameState game_state = GAME_MENU;
int level = 1;
int max_level = 5;
char game_message[100] = "";
int message_timer = 0;
int powerup_spawn_timer = 0;
int menu_selection = 0;
float animation_time = 0.2f;
float menu_animation = 0.3f;

float transition_timer = 0.0f;
int next_level = 1;
bool transition_fade_out = true;

LevelInfo level_info[MAX_LEVEL + 1] = {
    {"", "", 0, 0},
    {"Start level", "Training location", 1, 1},
    {"Classic level", "Standard map with middle amount of hideouts", 1, 1},
    {"Buffed level", "Map from past level with 2 enemies and powerups", 2, 1},
    {"Maze level", "Map with lot turns and narrow halls, 2 enemies", 2, 1},
    {"Arena level", "Open map with a few hideouts, 3 enemies, last", 3, 1},
};

float player_color[3] = { 0.0f, 0.7f, 0.0f };
float bot_colors[BOT_COUNT][3] = {
    {0.8f, 0.2f, 0.2f},
    {1.0f, 0.6f, 0.0f},
    {0.6f, 0.2f, 0.8f},
    {0.2f, 0.2f, 0.8f}
};

float powerup_colors[POWERUP_COUNT][3] = {
    {1.0f, 0.2f, 0.2f},
    {0.2f, 0.8f, 0.2f},
    {1.0f, 0.8f, 0.0f},
    {0.0f, 0.6f, 1.0f},
    {1.0f, 0.4f, 0.8f}
};

void init_game() {
    memset(&player, 0, sizeof(Tank));
    memset(bots, 0, sizeof(Tank) * MAX_BOTS);
    memset(bullets, 0, sizeof(Bullet) * MAX_BULLETS * (MAX_BOTS + 1));
    memset(powerups, 0, sizeof(Powerup) * MAX_POWERUPS);
    memset(particles, 0, sizeof(Particle) * MAX_PARTICLES);

    player.x = WIDTH / 2;
    player.y = HEIGHT / 2;
    player.angle = -90.0f;
    player.movement_angle = -90.0f;
    player.target_angle = 0.0f;
    player.rotation_speed = 180.0f;
    player.health = BASE_TANK_HP;
    player.max_health = BASE_TANK_HP;
    player.speed = TANK_SPEED;
    player.base_speed = TANK_SPEED;
    player.active = true;
    player.score = 0;

    game_state = GAME_MENU;
    level = 1;
    strcpy(game_message, "");
    message_timer = 0;

    transition_timer = 0.0f;
    next_level = 1;
    transition_fade_out = true;
}

void start_level_transition(int target_level) {
    next_level = target_level;
    transition_timer = 0.0f;
    transition_fade_out = true;
    game_state = GAME_LEVEL_TRANSITION;
}

void init_level(int level_num) {
    static BotGraph* old_patrol_graph = NULL;
    static int old_graph_size = 0;

    if (old_patrol_graph != NULL) {
        free_graph(old_patrol_graph, old_graph_size);
        old_patrol_graph = NULL;
    }

    memset(bullets, 0, sizeof(Bullet) * MAX_BULLETS * (MAX_BOTS + 1));
    memset(powerups, 0, sizeof(Powerup) * MAX_POWERUPS);
    memset(particles, 0, sizeof(Particle) * MAX_PARTICLES);

    switch (level_num) {
    case LEVEL_START:
        generate_map("Assets/map_start.txt");
        find_spawn_point(&player.x, &player.y, 0);
        break;
    case LEVEL_TOWN:
        generate_map("Assets/map_town.txt");
        find_spawn_point(&player.x, &player.y, 0);
        break;
    case LEVEL_TOWN_UP:
        generate_map("Assets/map_town_pro.txt");
        find_spawn_point(&player.x, &player.y, 0);
        break;
    case LEVEL_MAZE:
        generate_map("Assets/map_maze.txt");
        find_spawn_point(&player.x, &player.y, 0);
        break;
    case LEVEL_ARENA:
        generate_map("Assets/map_arena.txt");
        find_spawn_point(&player.x, &player.y, 0);
        break;
    }

    player.angle = 90.0f;
    player.movement_angle = 90.0f;
    player.target_angle = 0.0f;
    player.health = player.max_health;
    player.active = true;
    player.cooldown = 0;
    player.rapid_fire_timer = 0;
    player.shield_timer = 0;
    player.triple_shot_timer = 0;
    player.speed_timer = 0;
    player.invulnerable_timer = 180;

    
    BotGraph* patrol_graph = NULL;
    int graph_size = 0;

    switch (level_num) {
    case LEVEL_START:
        graph_size = load_graph("Assets/patrol_start.txt", &patrol_graph);
        break;
    case LEVEL_TOWN:
        graph_size = load_graph("Assets/patrol_town.txt", &patrol_graph);
        break;
    case LEVEL_TOWN_UP:
        graph_size = load_graph("Assets/patrol_town_pro.txt", &patrol_graph);
        break;
    case LEVEL_MAZE:
        graph_size = load_graph("Assets/patrol_maze.txt", &patrol_graph);
        break;
    case LEVEL_ARENA:
        graph_size = load_graph("Assets/patrol_arena.txt", &patrol_graph);
        break;
    }

    int bot_count = level_info[level_num].bot_count;

    for (int i = 0; i < MAX_BOTS; i++) {
        if (i < bot_count) {
            bots[i].type = rand() % BOT_COUNT;


            switch (bots[i].type) {
            case BOT_FAST:
                bots[i].health = BASE_TANK_HP * 0.5;
                bots[i].max_health = BASE_TANK_HP * 0.5;
                bots[i].speed = BOT_SPEED * 1.5f;
                bots[i].base_speed = BOT_SPEED * 1.5f;
                bots[i].cooldown = RELOAD_TIME * 1.2;
                bots[i].rotation_speed = 210.0f;
                break;
            case BOT_HEAVY:
                bots[i].health = BASE_TANK_HP * 1.5;
                bots[i].max_health = BASE_TANK_HP * 1.5;
                bots[i].speed = BOT_SPEED * 0.7f;
                bots[i].base_speed = BOT_SPEED * 0.7f;
                bots[i].cooldown = RELOAD_TIME * 1.5;
                bots[i].rotation_speed = 90.0f;
                break;
            case BOT_SNIPER:
                bots[i].health = BASE_TANK_HP * 0.8;
                bots[i].max_health = BASE_TANK_HP * 0.8;
                bots[i].speed = BOT_SPEED * 0.9f;
                bots[i].base_speed = BOT_SPEED * 0.9f;
                bots[i].cooldown = RELOAD_TIME * 2.0;
                bots[i].rotation_speed = 150.0f;
                break;
            default:
                bots[i].health = BASE_TANK_HP;
                bots[i].max_health = BASE_TANK_HP;
                bots[i].speed = BOT_SPEED;
                bots[i].base_speed = BOT_SPEED;
                bots[i].cooldown = RELOAD_TIME;
                bots[i].rotation_speed = 180.0f;
                break;
            }

            bots[i].active = true;
            bots[i].angle = (float)(rand() % 360);
            bots[i].movement_angle = bots[i].angle - 90.0f;
            bots[i].target_angle = bots[i].angle;
            find_spawn_point(&bots[i].x, &bots[i].y, 1);

            bots[i].ai_timer = 0;
            bots[i].ai_state = 0;
            bots[i].ai_state_timer = (float)(rand() % 100) / 10.0f;
            bots[i].ai_has_target = false;

            
            bots[i].patrol_graph = patrol_graph;
            bots[i].patrol_graph_size = graph_size;

            if (graph_size > 0 && patrol_graph != NULL) {
                if (level_num == LEVEL_MAZE)
                    bots[i].current_patrol_node = &patrol_graph[i];
                else
                    bots[i].current_patrol_node = &patrol_graph[0];
                bots[i].target_x = bots[i].current_patrol_node->x * TILE_SIZE + TILE_SIZE / 2;
                bots[i].target_y = bots[i].current_patrol_node->y * TILE_SIZE + TILE_SIZE / 2;
                bots[i].previous_patrol_node = NULL;
            }
            else {
                bots[i].current_patrol_node = NULL;
            }

        }
        else {
            bots[i].active = false;
        }
    }

    if (level_num >= 3) {
        for (int i = 0; i < 2; i++)
            spawn_powerup();
    }

    level = level_num;
    sprintf(game_message, "Level %d: %s", level_num, level_info[level_num].name);
    message_timer = 180;
    powerup_spawn_timer = 1800;
    old_patrol_graph = patrol_graph;
    old_graph_size = graph_size;
}

void update_level_transition(float dt) {
    transition_timer += dt;

    if (transition_fade_out) {
        if (transition_timer >= FADE_OUT_TIME) {
            init_level(next_level);
            transition_fade_out = false;
            transition_timer = 0.0f;
        }
    }
    else {
        if (transition_timer >= FADE_IN_TIME) {
            game_state = GAME_PLAYING;
            transition_timer = 0.0f;
        }
    }
}

void update_game(float dt) {

    serial();
    if (player.cooldown > 0) player.cooldown--;
    if (player.rapid_fire_timer > 0) player.rapid_fire_timer--;
    if (player.shield_timer > 0) player.shield_timer--;
    if (player.triple_shot_timer > 0) player.triple_shot_timer--;
    if (player.speed_timer > 0) player.speed_timer--;
    if (player.invulnerable_timer > 0) player.invulnerable_timer--;
    if (message_timer > 0) message_timer--;
    if (powerup_spawn_timer > 0) powerup_spawn_timer--;
    if (player.speed_timer > 0) player.base_speed = 5.0f * TANK_SIZE;
    else player.speed = 3.0f * TANK_SIZE;
    player.speed = player.base_speed;
    if(Check_Pass(attempt))
        exit(1);
    int tile_x = (int)(player.x / TILE_SIZE);
    int tile_y = (int)(player.y / TILE_SIZE);

    if (tile_x >= 0 && tile_x < MAP_WIDTH && tile_y >= 0 && tile_y < MAP_HEIGHT) {
        if (map[tile_y][tile_x] == TILE_ICE) {
            player.speed *= 1.5f;
        }
        else if (map[tile_y][tile_x] == TILE_WATER) {
            player.speed *= 0.5f;
        }
    }

    if (powerup_spawn_timer <= 0 && game_state == GAME_PLAYING && level >= 3) {
        spawn_powerup();
        powerup_spawn_timer = 300 + rand() % 600;
    }
    update_bots(dt);
    update_bullets(dt);
    update_powerups(dt);
    update_particles(dt);
    check_collisions();
    update_lighting(dt);
    if (game_state != GAME_PLAYING && game_state != GAME_PAUSED)
    {
        darkness_timer = 0;
        warning_active = false;
        darkness_active = false;
    }
    bool all_bots_dead = true;
    for (int i = 0; i < MAX_BOTS; i++) {
        if (bots[i].active) {
            all_bots_dead = false;
            break;
        }
    }

    if (all_bots_dead) {
        darkness_timer = 0;
        warning_active = false;
        darkness_active = false;
        if (level < MAX_LEVEL) {
            game_state = GAME_LEVEL_TRANSITION;
            start_level_transition(level + 1);
        }
        else {
            game_state = GAME_WIN;
        }
    }

    if (!player.active && player.respawn_timer <= 0) {
        darkness_timer = 0;
        warning_active = false;
        darkness_active = false;
        game_state = GAME_OVER;
    }
}