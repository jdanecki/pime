#include "tools.h"

Knife::Knife(InventoryElement * el1, InventoryElement * el2) : ProductServer(el1, el2, PROD_KNIFE, Form_solid)
{
    /* actions = new Product_action[1];
     actions[0]=ACT_CUT;
 */
    actions = ACT_CUT;
}


ProductServer * Knife::createKnife(InventoryElement * el1, InventoryElement * el2)
{
    int id1 = el1->get_id();
    int id2 = el2->get_id();

    if ((id1 == ING_KNIFE_BLADE && id2 == ING_KNIFE_HANDLE) || (id1 == ING_KNIFE_HANDLE && id2 == ING_KNIFE_BLADE))
        return new Knife(el1, el2);
    printf(" wrong ingredients id1=%d id=%d\n", id1, id2);
    return nullptr;
}
