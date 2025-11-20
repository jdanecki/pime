#include "tools.h"

CLASS_NAME::CLASS_NAME(InventoryElement * el1, InventoryElement * el2) : ProductServer(el1, el2, PROD, Form_solid, 1)
{
    actions[0] = ACT_NOTHING;
}

ProductServer * CLASS_NAME::createCLASS_NAME(InventoryElement * el1, InventoryElement * el2)
{
    return new CLASS_NAME(el1, el2);
}
