#include "axe.h"

Axe::Axe(InventoryElement * el1, InventoryElement * el2) : ProductServer(el1, el2, PROD_AXE, Form_solid)
{
    /* actions = new Product_action[1];
     actions[0]=ACT_HIT;*/
    actions = ACT_HIT;
}

bool Axe::check_ing()
{
    int id1 = ings[0]->get_id();
    int id2 = ings[1]->get_id();

    if ((id1 == ING_AXE_BLADE && id2 == ING_AXE_HANDLE) || (id1 == ING_AXE_HANDLE && id2 == ING_AXE_BLADE))
        return true;
    printf(" wrong ingredients\n");
    return false;
}

ProductServer * createAxe(InventoryElement * el1, InventoryElement * el2)
{
    return new Axe(el1, el2);
}
