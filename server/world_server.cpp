#include <stdarg.h>
#include "world_server.h"
#include "elements_server.h"
#include "../core/world.h"
#include "player_server.h"
#include "networking.h"

unsigned long get_time_usec()
{
    struct timespec t;

    clock_gettime(CLOCK_MONOTONIC_RAW, &t);
    return (t.tv_sec * 1000000 + t.tv_nsec / 1000);
}

unsigned long get_time_ms()
{
    return get_time_usec() / 1000;
}

void update()
{
    if (!players->nr_elements)
        return;
    // CONSOLE_LOG("update: time=%ld\n", get_time_ms());
#if 0
    // TODO maybe in the future make it smarter
    for (int y = 0; y < WORLD_SIZE; y++)
    {
        for (int x = 0; x < WORLD_SIZE; x++)
        {
            chunk * c = world_table[y][x];
            if (!c)
                continue;
            for (InventoryElement * el : world_table[y][x]->beings)
            {
                BeingServer * b = dynamic_cast<BeingServer *>(el);
                b->tick();
            }
        }
    }
#else
    ListElement * pl_el = players->head;

    while (pl_el)
    {
        PlayerServer * pl = ((PlayerServer *)((PlayerClient *)pl_el)->player);
        int x = pl->location.chunk.map_x;
        int y = pl->location.chunk.map_y;

        chunk * c = world_table[y][x];
        if (!c)
            continue;
        for (InventoryElement * el : world_table[y][x]->beings)
        {
            BeingServer * b = dynamic_cast<BeingServer *>(el);
            b->tick();
        }
        pl_el = pl_el->next;
    }
#endif
}
