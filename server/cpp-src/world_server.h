#ifndef WORLD_SERVER_H
#define WORLD_SERVER_H

#include "../../core/world.h"
#include <ctime>

void generator();
int height_at(int chunk_x, int chunk_y, int x, int y);

void update();
extern void print_status(int l, const char * format, ...);
extern "C"
{
    void load_chunk(int x, int y);
}
unsigned long get_time_ms();
#endif
