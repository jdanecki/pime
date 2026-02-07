#include <cstdio>
#include <cmath>

#include "world.h"
#include "alchemist/elements/inventory_element.h"
#include "alchemist/item_location.h"
#include "alchemist/object.h"
#include "player.h"
#include "chunk.h"
#include "world_params.h"

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
    //    CONSOLE_LOG("add_object_to_world: %s %ld @ (%f,%f)\n", object->get_name(), object->get_id(), (int)location.tag, location.chunk.x, location.chunk.y);
    switch (location.tag)
    {
        case ItemLocation::Tag::Chunk:
        {
            chunk * ch = world_table[location.chunk.map_y][location.chunk.map_x];
            if (ch)
                ch->add_object(object, location.chunk.x, location.chunk.y);
            break;
        }
        case ItemLocation::Tag::Player:
            abort();
    }
}

void add_object_to_world(InventoryElement * object)
{
    add_object_to_world(object, object->location);
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

InventoryElement * find_closest_item(InventoryElement * from, float * dist, Class_id cid)
{
    *dist = -1;
    if (from->location.tag == ItemLocation::Tag::Player)
        abort();

    float min_distance_target = 3 * CHUNK_SIZE;
    InventoryElement * target = nullptr;
    for (int y = from->location.chunk.map_y - 1; y < from->location.chunk.map_y + 2; y++)
        for (int x = from->location.chunk.map_x - 1; x < from->location.chunk.map_x + 2; x++)
        {
            if (y > -1 && y < WORLD_SIZE && x > -1 && x < WORLD_SIZE)
            {
                chunk * ch = world_table[y][x];
                if (!ch)
                    continue;
                for (InventoryElement * o : ch->objects)
                {
                    if (o->c_id == cid)
                    {
                        float ox = o->location.get_world_x();
                        float oy = o->location.get_world_y();
                        float l = distance(ox, oy, from->location.get_world_x(), from->location.get_world_y());
                        if (l < min_distance_target)
                        {
                            min_distance_target = l;
                            *dist = min_distance_target;
                            target = o;
                        }
                    }
                }
            }
        }
    return target;
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
