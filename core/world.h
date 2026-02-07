#ifndef WORLD_H
#define WORLD_H

#include "chunk.h"

class Player;

extern chunk * world_table[WORLD_SIZE][WORLD_SIZE];
enum Chunk_state
{
    CHUNK_NOT_LOADED,
    CHUNK_LOADING,
    CHUNK_LOADED
};

extern Chunk_state loaded_chunks[WORLD_SIZE][WORLD_SIZE];

void remove_from_chunks(InventoryElement * object);
void add_object_to_world(InventoryElement * object, ItemLocation location);
void add_object_to_world(InventoryElement * object);
void show_chunk(ItemLocation loc);
InventoryElement * find_in_world(ItemLocation * loc, size_t uid);
InventoryElement * get_item_at_ppos(Player * player);
InventoryElement * find_closest_item(InventoryElement * from, float * dist, Class_id cid);
#endif
