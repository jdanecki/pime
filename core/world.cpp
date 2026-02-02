#include <cstdio>

#include "world.h"
#include "alchemist/item_location.h"
#include "alchemist/object.h"
#include "player.h"

chunk * world_table[WORLD_SIZE][WORLD_SIZE];
Chunk_state loaded_chunks[WORLD_SIZE][WORLD_SIZE];

void remove_from_chunks(InventoryElement * object)
{
    if (world_table[(int)object->location.chunk.map_y][(int)object->location.chunk.map_x])
        world_table[(int)object->location.chunk.map_y][(int)object->location.chunk.map_x]->remove_object(object);
}

void add_object_to_world(InventoryElement * object, ItemLocation location)
{
    object->location = location;
    CONSOLE_LOG("add_object_to_world: %s %ld @ (%f,%f)\n", object->get_name(), object->get_id(), (int)location.tag, location.chunk.x, location.chunk.y);
    switch (location.tag)
    {
        case ItemLocation::Tag::Chunk:
            if (world_table[(int)location.chunk.map_y][(int)location.chunk.map_x])
                world_table[(int)location.chunk.map_y][(int)location.chunk.map_x]->add_object(object, location.chunk.x, location.chunk.y);
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
            world_table[(int)loc.chunk.map_y][(int)loc.chunk.map_x]->show();
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

InventoryElement * get_item_at_ppos(Player * player)
{
    if (player->location.tag == ItemLocation::Tag::Player)
        abort();

    int top = player->location.chunk.map_y - 1;
    if (top < 0)
        top = 0;
    int bottom = player->location.chunk.map_y + 1;
    if (bottom >= WORLD_SIZE)
        bottom = WORLD_SIZE - 1;
    int left = player->location.chunk.map_x - 1;
    if (left < 0)
        left = 0;
    int right = player->location.chunk.map_x + 1;
    if (right >= WORLD_SIZE)
        right = WORLD_SIZE - 1;

    // CONSOLE_LOG("left=%d right=%d top=%d bottom=%d\n", left, right, top, bottom);
    for (unsigned int cy = top; cy <= bottom; ++cy)
    {
        for (unsigned int cx = left; cx <= right; ++cx)
        {
            chunk * ch = world_table[cy][cx];
            if (!ch)
                continue;
            for (InventoryElement * el : ch->objects)
            {
                if (el->c_id == Class_Player)
                    continue;
                if (el->check_rect(player))
                {
                    return el;
                }
            }
        }
    }
    return nullptr;
}

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