#include "tools.h"

Fire::Fire(InventoryElement * el1, InventoryElement * el2) : ProductServer(el1, el2, PROD_FIRE, Form_solid)
{
    actions = ACT_FIRE;
}

ProductServer * Fire::createFire(InventoryElement * el1, InventoryElement * el2)
{
    int id1 = el1->get_id();
    int id2 = el2->get_id();

    if ((id1 == ING_TINDER && id2 == ING_STICK) || (id2 == ING_TINDER && id1 == ING_STICK))
        return new Fire(el1, el2);
    printf(" wrong ingredients\n");
    return nullptr;
}
