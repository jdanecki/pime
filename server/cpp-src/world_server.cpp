#include "world_server.h"
#include "elements_server.h"
#include <stdarg.h>

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
#if 1
    // TODO maybe in the future make it smarter
    for (int y = 0; y < WORLD_SIZE; y++)
    {
        for (int x = 0; x < WORLD_SIZE; x++)
        {
            chunk * c = world_table[y][x];
            if (!c)
                continue;
            for (InventoryElement* el : world_table[y][x]->beings)
            {
                BeingServer * b = dynamic_cast<BeingServer *>(el);
                b->tick();
            }
        }
    }
#else
    chunk * c = world_table[128][128];
    if (!c)
        return;

    ListElement * el = world_table[128][128]->beings.head;
    while (el)
    {
        BeingServer * b = dynamic_cast<BeingServer *>(el->el);
        b->tick();
        // unsigned long ms=get_time_ms();
        // CONSOLE_LOG("tick: %llu:%llu ms\n", ms/1000, ms % 1000);
        el = el->next;
    }
#endif
}

