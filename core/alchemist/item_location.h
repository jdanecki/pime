#ifndef ITEM_LOCATION_H
#define ITEM_LOCATION_H

extern unsigned int get_world_pos(unsigned int chunk, unsigned int pos);

struct ItemLocation
{
    enum class Tag
    {
        Chunk,
        Player,
    };

    struct Chunk_Body
    {
        int32_t map_x;
        int32_t map_y;
        unsigned int x;
        unsigned int y;
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
            CONSOLE_LOG("map_x:%d map_y:%d x:%d y:%d\n", chunk.map_x, chunk.map_y, chunk.x, chunk.y);
        }
        else
            CONSOLE_LOG("player: %lu\n", player.id);
    }
    unsigned int get_world_x()
    {
        return get_world_pos(chunk.map_x, chunk.x);
    }
    unsigned int get_world_y()
    {
        return get_world_pos(chunk.map_y, chunk.y);
    }
    unsigned int get_x()
    {
        return chunk.x;
    }
    unsigned int get_y()
    {
        return chunk.y;
    }
};
#endif
