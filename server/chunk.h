#ifndef CHUNK_H
#define CHUNK_H

#include "../core/tiles.h"

void add_place(chunk * ch, Place_id id, int x, int y);
void add_element(chunk * ch, size_t id);
void add_plant(chunk * ch, size_t id);
void add_animal(chunk * ch, size_t id);
void show_chunk();
void load_chunks();

#endif // CHUNK_H
