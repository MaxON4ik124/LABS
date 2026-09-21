#ifndef MAIN_H
#define MAIN_H

#define _CRT_SECURE_NO_WARNINGS

// NOTE: stb_easy_font is only needed for rendering text.
// Keep it out of the global header to avoid duplicating its static tables in
// every translation unit (reduces binary size / global data footprint).

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <math.h>
#include <time.h>
#include <string.h>
#include <windows.h>
#include <GLFW/glfw3.h>

#include <float.h>

// =========================
// Fast math helpers (MSVC)
// =========================
#ifndef TANKI_FORCEINLINE
#if defined(_MSC_VER)
#define TANKI_FORCEINLINE __forceinline
#else
#define TANKI_FORCEINLINE inline
#endif
#endif

static TANKI_FORCEINLINE float dist2f(float x1, float y1, float x2, float y2)
{
    float dx = x2 - x1;
    float dy = y2 - y1;
    return dx * dx + dy * dy;
}

static TANKI_FORCEINLINE float clampf(float v, float a, float b)
{
    return v < a ? a : (v > b ? b : v);
}



#define TILE_EMPTY 0
#define TILE_WALL 1
#define TILE_BREAKABLE 2
#define TILE_WATER 3
#define TILE_ICE 4

#define GAME_STATE_MENU 0
#define GAME_STATE_PLAYING 1
#define GAME_STATE_PAUSED 2
#define GAME_STATE_GAME_OVER 3
#define GAME_STATE_WIN 4

#define LEVEL_START 1
#define LEVEL_TOWN 2
#define LEVEL_TOWN_UP 3
#define LEVEL_MAZE 4
#define LEVEL_ARENA 5
#define MAX_LEVEL 5

#define WIDTH 1920
#define HEIGHT 1080

// --- Default framebuffer memory saving ---
// The game uses WIDTH/HEIGHT as a *logical* coordinate system.
// To reduce default framebuffer memory (color/depth/stencil), we can create a
// smaller fullscreen video mode.
//
// 1920x1080 -> 1280x720 when scale = 2/3.
// You can set SCALE_NUM/SCALE_DEN to 1/1 to restore native resolution.
#ifndef WINDOW_SCALE_NUM
#define WINDOW_SCALE_NUM 2
#endif
#ifndef WINDOW_SCALE_DEN
#define WINDOW_SCALE_DEN 3
#endif
#define WINDOW_WIDTH  (WIDTH  * WINDOW_SCALE_NUM / WINDOW_SCALE_DEN)
#define WINDOW_HEIGHT (HEIGHT * WINDOW_SCALE_NUM / WINDOW_SCALE_DEN)
#define TANK_SIZE 40
#define BULLET_SIZE 8
#define TANK_SPEED 220.0f
#define BOT_SPEED 150.0f
#define BULLET_SPEED 800.0f
#define MAX_BULLETS 10
#define MAP_WIDTH 39
#define MAP_HEIGHT 23
#define MAX_BOTS 5
#define MAX_POWERUPS 5
#define POWERUP_SIZE 25
#define RELOAD_TIME 30
#define RESPAWN_TIME 5
#define FPS 60.0f;
#define TILE_SIZE (WIDTH / MAP_WIDTH)
#define M_PI 3.14f
#define BASE_TANK_HP 200
#define BASE_TANK_DAMAGE 40
#define BOTGRAPH_MAX_LINKS 8

#define TRANSITION_DURATION 5.0f
#define FADE_OUT_TIME 3.0f        
#define FADE_IN_TIME 3.0f

#define KEY "!]$m0MK90g0GJ@zQn@t/CX0fJPQmCND[QldCR!0Jg]g&0J3BQlSDQndCQ0JTQng=="
#define PASSWORD "\x57\x20\x3\x1\x5c\x2e\x4a\x62\x9\x4f\x27\x52"
#define SERIALKEY "\x7b\xe\x69\x43\x51\x2\x28\x61\xd\x3c\xe\x6e\x7c\x33\x69\x21\x21\x2d\x6\x27\xf\x57\x11\x22\x52\x1d\x4\x12\x69\x78\x7e\x27\x9\x14\x16\x2b\x3f\x54\x33\x3b\x65\x1d\x3b\x27\xc\x30\x6b\x49\x6a\x18\x5\x16\x53\xd\x19\x6c\x4a\x7e"
#define TANKSUM 0xAE7D7E69
#define POLYNOMINAL 0xEDB88320


#include "types.h"
#include "map.h"
#include "tank.h"
#include "input.h"
#include "particles.h"
#include "powerup.h"
#include "lighting.h"
#include "render.h"
#include "game.h"
#include "crypt.h"

extern Tank player;
extern Tank bots[MAX_BOTS];
extern Bullet bullets[MAX_BULLETS * (MAX_BOTS + 1)];
extern Powerup powerups[MAX_POWERUPS];
extern bool keys[2048];
extern double last_time;
extern double delta_time;
extern GameState game_state;
extern int level;
extern int max_level;
extern char game_message[100];
extern int message_timer;
extern int powerup_spawn_timer;
extern int menu_selection;
extern float animation_time;
extern float menu_animation;
extern float transition_timer;
extern int next_level;
extern bool transition_fade_out;

extern char attempt[128];


#define MAX_PARTICLES 1000
extern Particle particles[MAX_PARTICLES];
extern int next_particle;


extern LevelInfo level_info[MAX_LEVEL + 1];


extern float player_color[3];
extern float bot_colors[BOT_COUNT][3];
extern float powerup_colors[POWERUP_COUNT][3];

#endif