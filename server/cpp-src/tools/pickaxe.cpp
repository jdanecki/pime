#include "tools.h"

Pickaxe::Pickaxe(InventoryElement * el1, InventoryElement * el2) : ProductServer(el1, el2, PROD_PICKAXE, Form_solid)
{
    /* actions = new Product_action[1];
     actions[0]=ACT_HIT;*/
    actions = ACT_HIT;
}

ProductServer * Pickaxe::createPickaxe(InventoryElement * el1, InventoryElement * el2)
{
    int id1 = el1->get_id();
    int id2 = el2->get_id();

    if ((id1 == ING_PICKAXE_BLADE && id2 == ING_PICKAXE_HANDLE) || (id1 == ING_PICKAXE_HANDLE && id2 == ING_PICKAXE_BLADE))
        return new Pickaxe(el1, el2);
    printf(" wrong ingredients\n");
    return nullptr;
}
