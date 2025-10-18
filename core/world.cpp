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
            return world_table[loc->chunk.map_y][loc->chunk.map_x]->find_by_id(uid);
        case ItemLocation::Tag::Player:
            abort();
            return nullptr;
    }
    abort();
    return nullptr;
}

int get_world_x(ItemLocation loc)
{
    return get_world_pos(loc.chunk.map_x, loc.chunk.x);
}
int get_world_y(ItemLocation loc)
{
    return get_world_pos(loc.chunk.map_y, loc.chunk.y);
}

int get_world_pos(int chunk, int pos)
{
    return chunk * CHUNK_SIZE + pos;
}

int get_tile_at(ItemLocation loc)
{
    return world_table[loc.chunk.map_y][loc.chunk.map_x]->table[loc.chunk.y][loc.chunk.x].tile;
}

InventoryElement * get_item_at(ItemLocation loc)
{ //loc is player position
    if (loc.tag == ItemLocation::Tag::Player)
        abort();

    chunk * ch = world_table[loc.chunk.map_y][loc.chunk.map_x];
    if (!ch)
        return nullptr;
    for (InventoryElement* el: ch->objects)
    {
        if (el->check_rect(loc.chunk.x, loc.chunk.y, tile_size))
        {
            return el;
        }
    }
    return nullptr;
}

InventoryElement * get_item_at_ppos(Player * player)
{
    return get_item_at(player->location);
}

