#pragma once
#include "../core/tiles.h"
#include "ogl.h"
extern chunk * world_table[WORLD_SIZE][WORLD_SIZE];
extern OGL_Chunk * ogl_tiles[WORLD_SIZE][WORLD_SIZE];
extern size_t my_id;
extern OGL_Camera cam;
