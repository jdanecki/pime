#include "tools.h"
#include "../places/places.h"

Hut::Hut(InventoryElement * el1, InventoryElement * el2) : ProductServer(el1, el2, PROD_HUT, Form_solid, 0)
{
    // FIXME
    // actions = ACT_NOTHING;
}

ProductServer * Hut::Hut::createHut(InventoryElement * el1, InventoryElement * el2)
{
    int id1 = el1->get_id();
    int id2 = el2->get_id();

    if (id1 == ING_WALL && id2 == ING_WALL)
        return new Hut(el1, el2);
    CONSOLE_LOG(" wrong ingredients\n");
    return nullptr;
}

bool Hut::can_pickup()
{
    return false;
}

bool Hut::use_on(InventoryElement * object, Player * pl)
{
    CONSOLE_LOG("%s: use on %s\n", get_name(), object->get_name());
    switch (object->get_cid())
    {
        case Class_Animal:
        {
            CONSOLE_LOG("creating barn\n");
            chunk * ch = world_table[pl->location.chunk.map_y][pl->location.chunk.map_x];
            ch->add_object(create_place(PLACE_BARN), pl->location.chunk.x, pl->location.chunk.y);
            destroy(this);
            destroy(object);
            return true;
        }
        default:
            CONSOLE_LOG("Can't use %s with %s\n", get_name(), object->get_name());
            return false;
    }
}