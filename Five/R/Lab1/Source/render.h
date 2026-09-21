#ifndef RENDER_H
#define RENDER_H

#include "main.h"
#include "stb_easy_font.h"

void draw_map(void);
void draw_ui(void);
void draw_menu(void);
void draw_game_over(void);
void draw_win_screen(void);
void draw_pause_menu(void);
void render(void);
void draw_text(const char* text, float x, float y, float scale, float r, float g, float b);
void draw_level_transition(void);
#endif
