#include "tools.h"

Stick::Stick(InventoryElement * from) : IngredientServer(from, ING_STICK, Form_solid)
{
}

IngredientServer * Stick::createStick(InventoryElement * from)
{
    if (from->get_cid() == Class_Plant)
        return new Stick(from);
    else
        return nullptr;
}
