#include "tools.h"

Knife::Knife(InventoryElement * el1, InventoryElement * el2) : ProductServer(el1, el2, PROD_KNIFE, Form_solid)
{
    /* actions = new Product_action[1];
     actions[0]=ACT_CUT;
 */
    actions = ACT_CUT;
}

bool Knife::check_ing()
{
    int id1 = ings[0]->get_id();
    int id2 = ings[1]->get_id();

    if ((id1 == ING_KNIFE_BLADE && id2 == ING_KNIFE_HANDLE) || (id1 == ING_KNIFE_HANDLE && id2 == ING_KNIFE_BLADE))
        return true;
    printf(" wrong ingredients id1=%d id=%d\n", id1, id2);
    return false;
}

ProductServer * createKnife(InventoryElement * el1, InventoryElement * el2)
{
    return new Knife(el1, el2);
}
