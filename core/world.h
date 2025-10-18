#ifndef WORLD_H
#define WORLD_H

#include "tiles.h"

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
void show_chunk(ItemLocation loc);
InventoryElement * find_in_world(ItemLocation * loc, size_t uid);
int get_world_x(ItemLocation loc);
int get_world_y(ItemLocation loc);
int get_world_pos(int chunk, int pos);
InventoryElement * get_item_at(ItemLocation loc);
int get_tile_at(ItemLocation loc);

// Being ** get_being_at_ppos(Player * player);
// Being ** get_being_at(int chunk_x, int chunk_y, int x, int y);

Object ** get_object_at(int chunk_x, int chunk_y, int x, int y);
Object ** get_object_at_ppos(Player * player);

Plant ** get_plant_at(int chunk_x, int chunk_y, int x, int y);
Plant ** get_plant_at_ppos(Player * player);

Animal ** get_animal_at_ppos(Player * player);
Animal ** get_animal_at(int chunk_x, int chunk_y, int x, int y);

InventoryElement * get_item_at_ppos(Player * player);

void set_item_at(InventoryElement * item, int chunk_x, int chunk_y, int x, int y);
void set_item_at_ppos(InventoryElement * item, Player * player);

bool check_chunks();

extern int tile_size;
#endif
