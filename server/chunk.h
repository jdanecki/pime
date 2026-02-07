#ifndef CHUNK_H
#define CHUNK_H

#include "../core/chunk.h"

void add_place(chunk * ch, Place_id id, int x, int y);
void add_element(chunk * ch, size_t id);
void add_plant(chunk * ch, size_t id);
void add_animal(chunk * ch, size_t id);
void show_chunk();
void show_beings();
void load_chunks();
chunk * get_chunk(ItemLocation location);

#endif // CHUNK_H
