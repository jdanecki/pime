#include "tools.h"

Stick::Stick(InventoryElement * from) : IngredientServer(from, ING_STICK, Form_solid)
{
}

bool Stick::check_ing()
{
    if (el->get_base_cid() == Class_BasePlant)
        return new Stick(from);
    else
        return nullptr;
}

IngredientServer * createStick(InventoryElement * from)
{
    return new Stick(from);
}
