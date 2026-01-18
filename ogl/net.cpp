#include "../client-common/net.h"
#include "../core/tiles.h"
#include <cstdio>
#include <cstdlib>
#include <string.h>

void update_chunk(int32_t x, int32_t y, const chunk_table * data)
{
    printf("UPDATE CHUNK!!!\n");
    data = (chunk_table *)((char *)(data));
    if (!world_table[y][x])
    {
        world_table[y][x] = new chunk(x, y);
        memcpy(world_table[y][x]->table, &data[0], CHUNK_SIZE * CHUNK_SIZE * sizeof(int));
    }
}
void update_object(const ObjectData * data)
{
}
void update_item_location(LocationUpdateData data)
{
}
void create_object(const ObjectData * data)
{
}
void destroy_object(NetworkObject id, ItemLocation location)
{
}
void failed_craft()
{
}
void action_failed()
{
}
void knowledge_update(size_t pl_id, Class_id cid, int32_t id)
{
}
void checked_update(size_t pl_id, uintptr_t el)
{
}
