#ifndef ITEM_LOCATION_H
#define ITEM_LOCATION_H

#include <cstdint>
#include "object.h"
extern float get_world_pos(int, float);

struct ItemLocation
{
    enum class Tag
    {
        Chunk,
        Player,
    };

    struct Chunk_Body
    {
        int map_x;
        int map_y;
        float x;
        float y;
    };

    struct Player_Body
    {
        // Class_id c_id;
        uintptr_t id;
    };

    Tag tag;
    union
    {
        Chunk_Body chunk;
        Player_Body player;
    };
    /*    ItemLocation()
        {
            tag = ItemLocation::Tag::Chunk;
            chunk = {128, 128, 8, 8};
        }*/
    static ItemLocation center()
    {
        ItemLocation l;
        l.tag = ItemLocation::Tag::Chunk;
        l.chunk = {128, 128, 8, 8};
        return l;
    }
    void show()
    {
        if (tag == Tag::Chunk)
        {
            CONSOLE_LOG("map_x:%d map_y:%d x:%f y:%f\n", chunk.map_x, chunk.map_y, chunk.x, chunk.y);
        }
        else
            CONSOLE_LOG("player: %lu\n", player.id);
    }
    float get_world_x()
    {
        return get_world_pos(chunk.map_x, chunk.x);
    }
    float get_world_y()
    {
        return get_world_pos(chunk.map_y, chunk.y);
    }
    float get_x()
    {
        return chunk.x;
    }
    float get_y()
    {
        return chunk.y;
    }
};
#endif
