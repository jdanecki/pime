#include "tools.h"

Tinder::Tinder(InventoryElement * from) : IngredientServer(from, ING_TINDER, Form_solid)
{
}

IngredientServer * Tinder::createTinder(InventoryElement * from)
{
    if ((from->get_cid() == Class_Element) || (from->get_cid() == Class_Plant))
        return new Tinder(from);
    else
        return nullptr;
}
