#ifndef WORLD_SERVER_H
#define WORLD_SERVER_H

#include <ctime>

void generator();
int height_at(int chunk_x, int chunk_y, int x, int y);

void update();
extern void print_status(int l, const char * format, ...);
void load_chunk(int x, int y);
void check_and_load_chunk(int new_map_x, int new_map_y);

#endif
