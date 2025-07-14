#ifndef WORLD_H
#define WORLD_H

#include "tiles.h"

class Player;

extern char traversable_tiles[TILE_MAX_NUM];

extern chunk * world_table[WORLD_SIZE][WORLD_SIZE];

void remove_from_chunks(InventoryElement * object);

int get_tile_at(int chunk_x, int chunk_y, int x, int y);
enum game_tiles get_tile_at_ppos(Player * player);

// Being ** get_being_at_ppos(Player * player);
// Being ** get_being_at(int chunk_x, int chunk_y, int x, int y);

Object ** get_object_at(int chunk_x, int chunk_y, int x, int y);
Object ** get_object_at_ppos(Player * player);

Plant ** get_plant_at(int chunk_x, int chunk_y, int x, int y);
Plant ** get_plant_at_ppos(Player * player);

Animal ** get_animal_at_ppos(Player * player);
Animal ** get_animal_at(int chunk_x, int chunk_y, int x, int y);

InventoryElement * get_item_at_ppos(Player * player);
InventoryElement * get_item_at(int chunk_x, int chunk_y, int x, int y);

void set_item_at(InventoryElement * item, int chunk_x, int chunk_y, int x, int y);
void set_item_at_ppos(InventoryElement * item, Player * player);

#endif
