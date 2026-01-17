#ifndef TILES_H
#define TILES_H

#include "alchemist/el_list.h"
#include "world_params.h"

enum biomes
{
    BIOME_DESERT, // yellow
    BIOME_FOREST, // green with trees
    BIOME_PLAINS, // green
};
#define BIOMES 3
#define WORLD_SCALE 100.0

struct tile
{
    int tile;
};

const unsigned int TILE_SIZE = sizeof(struct tile);

typedef struct tile chunk_table[CHUNK_SIZE][CHUNK_SIZE];

class chunk
{
    int map_x, map_y;

  public:
    chunk_table table;
    // all elements are in objects in client
    // in server beings are on both lists
    ElementsList objects;
    ElementsList beings;

    void add_object(InventoryElement * object, int x, int y, bool front=true);
    void add_object(InventoryElement * object);
    void remove_object(InventoryElement * object);
    InventoryElement * find_by_id(size_t id);
    chunk(int map_x, int map_y);
    void show();
};

#endif
