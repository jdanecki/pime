#include "../core/player.h"
#include "../core/tiles.h"
#include "networking.h"
#include <cstring>

const NetClient * init()
{
    got_id(0, time(0));

    chunk_table table;
    for (int y = 0; y < CHUNK_SIZE; y++)
        for (int x = 0; x < CHUNK_SIZE; x++)
            table[y][x].tile = (game_tiles)(random_range(0, TILE_MAX_NUM - 1));

    table[5][5].tile = TILE_AIR;
    update_chunk(128, 128, (uint8_t *)table);
    return nullptr;
}

void network_tick(const NetClient * client)
{
}

void send_packet_move(const NetClient * client, int32_t x, int32_t y)
{
    player_server->move(x, y);
}
void send_packet_pickup(const NetClient * client, uintptr_t id)
{
}
void send_packet_drop(const NetClient * client, uintptr_t id)
{
}
void send_packet_item_used_on_object(const NetClient * client, uintptr_t iid, uintptr_t oid)
{
}
void send_packet_item_used_on_tile(const NetClient * client, uintptr_t iid, int32_t map_x, int32_t map_y, int32_t x, int32_t y)
{
}

void send_packet_craft(const NetClient * client, uintptr_t prod_id, uintptr_t ingredients_num, const uintptr_t * iid)
{
}

void notify_destroy(size_t id, ItemLocation location)
{
}

void update_location(size_t id, ItemLocation old_loc, ItemLocation new_loc)
{
}

void notify_knowledge(size_t pl_id, Class_id cid, int id)
{
}
void notify_checked(size_t pl_id, size_t el)
{
}
void notify_update(const InventoryElement * el)
{
}


