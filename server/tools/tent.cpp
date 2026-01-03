#include "tools.h"

Tent::Tent(InventoryElement * el1, InventoryElement * el2) : ProductServer(el1, el2, PROD_TENT, Form_solid, 0)
{
    //FIXME
    //actions = ACT_NOTHING;
}

ProductServer * Tent::createTent(InventoryElement * el1, InventoryElement * el2)
{
    int id1 = el1->get_id();
    int id2 = el2->get_id();

    if (id1 == ING_STICK && id2 == ING_STICK)
        return new Tent(el1, el2);
    CONSOLE_LOG(" wrong ingredients\n");
    return nullptr;
}
