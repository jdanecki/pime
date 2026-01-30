#ifndef WINDOW_H
#define WINDOW_H

extern int window_width;
extern int window_height;

int init_window(const char * title, int wx, int wy, bool resizable=true);
void close_graphics();
void clear_window();
unsigned int color(int r, int g, int b, int a);
void update_window_size();

#define STATUS_LINES (3 * 32)

extern float left_top_world_x;
extern float left_top_world_y;
void set_tile_size(int width, int height);
#endif // WINDOW_H
