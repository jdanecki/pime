#include "roasted_meat.h"

Roasted_meat::Roasted_meat(InventoryElement * el1, InventoryElement * el2) : ProductServer(el1, el2, PROD_ROASTED_MEAT, Form_solid)
{
    actions = ACT_NOTHING;
}

bool Roasted_meat::check_ing()
{
    int id1 = ings[0]->get_id();
    int id2 = ings[1]->get_id();

    if ((id1 == ING_MEAT && id2 == PROD_FIRE) || (id2 == ING_MEAT && id1 == PROD_FIRE))
        return true;
    printf(" wrong ingredients\n");
    return false;
}
