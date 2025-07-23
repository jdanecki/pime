#include "pickaxe.h"

PickAxe::PickAxe(InventoryElement * el1, InventoryElement * el2) : ProductServer(el1, el2, PROD_PICKAXE, Form_solid)
{
    /* actions = new Product_action[1];
     actions[0]=ACT_HIT;*/
    actions = ACT_HIT;
}

bool PickAxe::check_ing()
{
    int id1 = ings[0]->get_id();
    int id2 = ings[1]->get_id();

    if ((id1 == ING_PICKAXE_BLADE && id2 == ING_PICKAXE_HANDLE) || (id1 == ING_PICKAXE_HANDLE && id2 == ING_PICKAXE_BLADE))
        return true;
    printf(" wrong ingredients\n");
    return false;
}
