#include "hut.h"

Hut::Hut(InventoryElement * el1, InventoryElement * el2) : ProductServer(el1, el2, PROD_HUT, Form_solid)
{
    actions = ACT_NOTHING;
}

bool Hut::check_ing()
{
    int id1 = ings[0]->get_id();
    int id2 = ings[1]->get_id();

    if (id1 == ING_WALL && id2 == ING_WALL)
        return true;
    printf(" wrong ingredients\n");
    return false;
}
