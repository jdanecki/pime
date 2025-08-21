#include "tools.h"

Tinder::Tinder(InventoryElement * from) : IngredientServer(from, ING_TINDER, Form_solid)
{
}

bool Tinder::check_ing()
{
    if ((el->get_base_cid() == Class_BaseElement) || (el->get_base_cid() == Class_BasePlant))
        return new Tinder(from);
    else
        return nullptr;
}

IngredientServer * createTinder(InventoryElement * from)
{
    return new Tinder(from);
}
