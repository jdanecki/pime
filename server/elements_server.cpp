#include "elements_server.h"
#include "networking.h"

// #define TRACE_PLANTS 1

void destroy(InventoryElement * el)
{
    if (el->location.tag == ItemLocation::Tag::Chunk)
    {
        chunk * ch = world_table[(int)el->location.chunk.map_y][(int)el->location.chunk.map_x];
        if (ch)
        {
            ch->remove_object(el);
            notify_destroy(el);
        }
    }
}
