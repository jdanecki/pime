#include "tools.h"

CLASS_NAME::CLASS_NAME(InventoryElement * from) : IngredientServer(from, ING, Form_solid)
{
}

bool CLASS_NAME::check_ing()
{
    if ((el->get_base_cid() == Class_BaseElement) || (el->get_base_cid() == Class_BasePlant))
        return new CLASS_NAME(from);
    else
        return nullptr;
}

IngredientServer * createCLASS_NAME(InventoryElement * from)
{
    return new CLASS_NAME(from);
}
