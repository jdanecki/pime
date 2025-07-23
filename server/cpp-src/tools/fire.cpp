#include "fire.h"

Fire::Fire(InventoryElement * el1, InventoryElement * el2) : ProductServer(el1, el2, PROD_FIRE, Form_solid)
{
    actions = ACT_FIRE;
}

bool Fire::check_ing()
{
    int id1 = ings[0]->get_id();
    int id2 = ings[1]->get_id();

    if ((id1 == ING_TINDER && id2 == ING_STICK) || (id2 == ING_TINDER && id1 == ING_STICK))
        return true;
    printf(" wrong ingredients\n");
    return false;
}

ProductServer *createFire(InventoryElement *el1, InventoryElement *el2) { return new Fire(el1, el2); }
