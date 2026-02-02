#ifndef ITEM_LOCATION_H
#define ITEM_LOCATION_H

#include <cstdint>

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
    static ItemLocation center();
    void show();
    float get_world_x();
    float get_world_y();
    float get_tile_x();
    float get_tile_y();
    float get_world_pos(int, float); // position in tiles
    bool operator!=(const ItemLocation & other);
};
#endif
