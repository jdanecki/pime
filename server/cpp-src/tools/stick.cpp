#include "tools.h"

Stick::Stick(InventoryElement * from) : IngredientServer(from, ING_STICK, Form_solid)
{
}

IngredientServer * Stick::createStick(InventoryElement * from)
{
    if (from->get_base_cid() == Class_BasePlant)
        return new Stick(from);
    else
        return nullptr;
}
