#include "tools.h"

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
