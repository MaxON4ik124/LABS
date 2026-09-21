#ifndef TYPES_H
#define TYPES_H
#include "main.h"

typedef enum {
    POWERUP_HEALTH,
    POWERUP_SPEED,
    POWERUP_RAPID_FIRE,
    POWERUP_SHIELD,
    POWERUP_TRIPLE_SHOT,
    POWERUP_COUNT
} PowerupType;


typedef enum {
    GAME_MENU,
    GAME_PLAYING,
    GAME_PAUSED,
    GAME_OVER,
    GAME_WIN,
    GAME_LEVEL_TRANSITION
} GameState;

typedef struct BotGraph {
    int id;
    int x;
    int y;
    int next_index;                 // how many outgoing links
    int nextinds[BOTGRAPH_MAX_LINKS]; // indices of next nodes (1..N as stored in patrol_*.txt)
} BotGraph;


typedef enum {
    BOT_REGULAR,
    BOT_FAST,
    BOT_HEAVY,
    BOT_SNIPER,
    BOT_COUNT
} BotType;

typedef struct {
    float x, y;
    float angle;
    float movement_angle;
    float target_angle;
    float rotation_speed;
    int health;
    int max_health;
    bool shooting;
    int cooldown;
    float speed;
    float base_speed;
    int rapid_fire_timer;
    bool active;
    int respawn_timer;
    int score;
    int invulnerable_timer;
    int shield_timer;
    int triple_shot_timer;
    int speed_timer;
    BotType type;
    float target_x, target_y;
    float ai_timer;
    float ai_state_timer;
    int ai_state;
    bool ai_has_target;
    BotGraph* previous_patrol_node;
    BotGraph* current_patrol_node;
    BotGraph* patrol_graph;
    int patrol_graph_size;
    bool avoiding_obstacle;
    float avoidance_timer;
    int avoidance_direction;
} Tank;



typedef struct {
    float x, y;
    float dx, dy;
    bool active;
    bool is_player;
    int damage;
    float life_time;
} Bullet;

typedef struct {
    float x, y;
    PowerupType type;
    bool active;
    int timer;
    float rotation;
    float scale;
    bool scaling_up;
} Powerup;


typedef struct {
    int x, y;
    float distance;
    bool valid;
} SpawnPoint;

typedef struct {
    float x, y;
    float dx, dy;
    float life;
    float max_life;
    float size;
    uint8_t r, g, b;
    bool active;
} Particle;


typedef struct {
    char name[50];
    char description[100];
    int bot_count;
    int difficulty;
} LevelInfo;


#endif