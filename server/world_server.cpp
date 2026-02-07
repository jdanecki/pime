#include <stdarg.h>
#include "world_server.h"
#include "elements_server.h"
#include "../core/world.h"
#include "player_server.h"
#include "networking.h"

void update()
{
    if (!players->nr_elements)
        return;
//  CONSOLE_LOG("update: time=%ld\n", get_time_ms());
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
        for (int y = pl->location.chunk.map_y - 1; y <= pl->location.chunk.map_y + 1; y++)
        {
            for (int x = pl->location.chunk.map_x - 1; x <= pl->location.chunk.map_x + 1; x++)
            {
                if (x < 0 || x >= WORLD_SIZE || y < 0 || y >= WORLD_SIZE)
                    continue;
                chunk * c = world_table[y][x];
                if (!c)
                    continue;
                auto it = c->beings.begin();
                while (it != c->beings.end())
                {
                    InventoryElement * el = *it;
                    ++it;

                    BeingServer * b = dynamic_cast<BeingServer *>(el);
                    if (b)
                    {
                        b->tick();
                    }
                    else
                    {
                        CONSOLE_LOG("world_server::update: unknown being @ chunk_map:(%d,%d):%f,%f x/y:%d,%d uid=%x cid=%ld\n", el->location.chunk.map_x, el->location.chunk.map_y,
                            el->location.chunk.x, el->location.chunk.y, x, y, el->get_uid(), el->get_cid());
                    }
                }
            }
        }
        pl_el = pl_el->next;
    }
#endif
}

void check_and_load_chunk(int new_map_x, int new_map_y)
{
    int size = 5;
    for (int cy = new_map_y - size; cy <= new_map_y + size; cy++)
    {
        if (cy >= 0 && cy < WORLD_SIZE)
        {
            for (int cx = new_map_x - size; cx <= new_map_x + size; cx++)
            {
                if (cx >= 0 && cx < WORLD_SIZE)
                {
                    if (!world_table[cy][cx])
                        load_chunk(cx, cy);
                }
            }
        }
    }
}