#include "tools.h"

CLASS_NAME::CLASS_NAME(InventoryElement * el1, InventoryElement * el2) : ProductServer(el1, el2, PROD, Form_solid)
{
    actions = ACT_NOTHING;
}

bool CLASS_NAME::check_ing()
{
    int id1 = ings[0]->get_id();
    int id2 = ings[1]->get_id();

    if (id1 == ING_1 && id2 == ING_2)
        return new CLASS_NAME(el1, el2);
    CONSOLE_LOG(" wrong ingredients\n");
    return nullptr;
}

ProductServer * CLASS_NAME::createCLASS_NAME(InventoryElement * el1, InventoryElement * el2)
{
    return new CLASS_NAME(el1, el2);
}
