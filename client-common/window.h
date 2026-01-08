#ifndef WINDOW_H
#define WINDOW_H

#include "backend.inl"

#include "../core/world_params.h"

extern int window_width;
extern int window_height;

int init_window(const char *title, int wx, int wy);
void close_graphics();
void clear_window();
unsigned int color(int r, int g, int b, int a);
void update_window_size();

#define PANEL_WINDOW 540
#define STATUS_LINES (2 * 32)
extern int tile_size;

#endif // WINDOW_H
