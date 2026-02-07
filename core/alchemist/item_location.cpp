#include "item_location.h"
#include "object.h"
#include "../world_params.h"
#include <cmath>

ItemLocation ItemLocation::center()
{
    ItemLocation l;
    l.tag = ItemLocation::Tag::Chunk;
    l.chunk = {128, 128, 8.5, 8.5};
    return l;
}
void ItemLocation::show()
{
    if (tag == Tag::Chunk)
    {
        CONSOLE_LOG("map_x:%d map_y:%d x:%f y:%f\n", chunk.map_x, chunk.map_y, chunk.x, chunk.y);
    }
    else
        CONSOLE_LOG("player: %lu\n", player.id);
}
float ItemLocation::get_world_x()
{
    return get_world_pos(chunk.map_x, chunk.x);
}
float ItemLocation::get_world_y()
{
    return get_world_pos(chunk.map_y, chunk.y);
}
float ItemLocation::get_tile_x()
{
    return chunk.x;
}
float ItemLocation::get_tile_y()
{
    return chunk.y;
}

float ItemLocation::get_world_pos(int chunk, float pos)
{
    return chunk * CHUNK_SIZE + pos;
}

bool ItemLocation::operator!=(const ItemLocation & other)
{
    if (tag != other.tag)
        return true;
    switch (tag)
    {
        case Tag::Chunk:
            return chunk.map_x != other.chunk.map_x || chunk.map_y != other.chunk.map_y || chunk.x != other.chunk.x || chunk.y != other.chunk.y;
        case Tag::Player:
            return player.id != other.player.id;
    }
    return false;
}

float distance(float x1, float y1, float x2, float y2)
{
    float dx = x1 - x2;
    float dy = y1 - y2;
    return sqrt(dx * dx + dy * dy);
}

void ItemLocation::set_chunk(int map_x, int map_y, float x, float y)
{
    tag = Tag::Chunk;
    chunk.map_x = map_x;
    chunk.map_y = map_y;
    chunk.x = x;
    chunk.y = y;
}