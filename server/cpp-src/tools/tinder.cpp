#include "tools.h"

Tinder::Tinder(InventoryElement * from) : IngredientServer(from, ING_TINDER, Form_solid)
{
}

IngredientServer * Tinder::createTinder(InventoryElement * from)
{
    if ((from->get_base_cid() == Class_BaseElement) || (from->get_base_cid() == Class_BasePlant))
        return new Tinder(from);
    else
        return nullptr;
}
