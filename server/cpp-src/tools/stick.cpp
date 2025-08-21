#include "tools.h"

Stick::Stick(InventoryElement * from) : IngredientServer(from, ING_STICK, Form_solid)
{
}

bool Stick::check_ing()
{
    if (el->get_base_cid() == Class_BasePlant)
        return true;
    else
        return false;
}

IngredientServer * createStick(InventoryElement * from)
{
    return new Stick(from);
}
