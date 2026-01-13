#include "world.h"
#include "player.h"

int tile_size;
chunk * world_table[WORLD_SIZE][WORLD_SIZE];
Chunk_state loaded_chunks[WORLD_SIZE][WORLD_SIZE];

void remove_from_chunks(InventoryElement * object)
{
    if (world_table[object->location.chunk.map_y][object->location.chunk.map_x])
        world_table[object->location.chunk.map_y][object->location.chunk.map_x]->remove_object(object);
}

void add_object_to_world(InventoryElement * object, ItemLocation location)
{
    object->location = location;
    switch (location.tag)
    {
        case ItemLocation::Tag::Chunk:
            if (world_table[location.chunk.map_y][location.chunk.map_x])
                world_table[location.chunk.map_y][location.chunk.map_x]->add_object(object, location.chunk.x, location.chunk.y);
            break;
        case ItemLocation::Tag::Player:
            abort();
    }
}

void show_chunk(ItemLocation loc)
{
    switch (loc.tag)
    {
        case ItemLocation::Tag::Chunk:
            world_table[loc.chunk.map_y][loc.chunk.map_x]->show();
            break;
    }
}

InventoryElement * find_in_world(ItemLocation * loc, size_t uid)
{
    switch (loc->tag)
    {
        case ItemLocation::Tag::Chunk:
        {
            for (int y = loc->chunk.map_y - 1; y < loc->chunk.map_y + 2; y++)
                for (int x = loc->chunk.map_x - 1; x < loc->chunk.map_x + 2; x++)
                {
                    if (y > -1 && y < WORLD_SIZE && x > -1 && x < WORLD_SIZE)
                    {
                        InventoryElement * el = world_table[y][x]->find_by_id(uid);
                        if (el)
                            return el;
                    }
                }
            return nullptr;
        }
        case ItemLocation::Tag::Player:
            abort();
            return nullptr;
    }
    abort();
    return nullptr;
}

int get_tile_at(ItemLocation loc)
{
    return world_table[loc.chunk.map_y][loc.chunk.map_x]->table[loc.chunk.y][loc.chunk.x].tile;
}

InventoryElement * get_item_at(ItemLocation loc)
{ // loc is player position
    if (loc.tag == ItemLocation::Tag::Player)
        abort();

    unsigned int left_chunk_x, right_chunk_x, top_chunk_y, bottom_chunk_y, left_top_world_x, left_top_world_y;
    get_chunks_around(loc, &left_chunk_x, &right_chunk_x, &top_chunk_y, &bottom_chunk_y, &left_top_world_x, &left_top_world_y);

    for (unsigned int cy = top_chunk_y; cy <= bottom_chunk_y; ++cy)
    {
        for (unsigned int cx = left_chunk_x; cx <= right_chunk_x; ++cx)
        {
            chunk * ch = world_table[cy][cx];
            if (!ch)
                return nullptr;
            for (InventoryElement * el : ch->objects)
            {
                if (el->c_id == Class_Player)
                    continue;
                if (el->check_rect(loc.get_world_x(), loc.get_world_y(), tile_size))
                {
                    return el;
                }
            }
        }
    }
    return nullptr;
}

InventoryElement * get_item_at_ppos(Player * player)
{
    return get_item_at(player->location);
}

unsigned int get_world_pos(unsigned int chunk, unsigned int pos)
{
    return chunk * CHUNK_SIZE + pos;
}

void get_chunks_around(ItemLocation loc, unsigned int * left_chunk_x, unsigned int * right_chunk_x, unsigned int * top_chunk_y, unsigned int * bottom_chunk_y, unsigned int * left_top_world_x,
    unsigned int * left_top_world_y)
{
    unsigned int player_world_x = loc.get_world_x();
    unsigned int player_world_y = loc.get_world_y();

    *left_top_world_x = player_world_x - CHUNK_SIZE / 2;
    *left_top_world_y = player_world_y - CHUNK_SIZE / 2;

    *left_chunk_x = *left_top_world_x / CHUNK_SIZE;
    *right_chunk_x = (*left_top_world_x + CHUNK_SIZE - 1) / CHUNK_SIZE;

    *top_chunk_y = *left_top_world_y / CHUNK_SIZE;
    *bottom_chunk_y = (*left_top_world_y + CHUNK_SIZE - 1) / CHUNK_SIZE;
}
