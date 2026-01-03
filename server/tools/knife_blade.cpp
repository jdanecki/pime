#include "tools.h"

KnifeBlade::KnifeBlade(InventoryElement * from) : IngredientServer(from, ING_KNIFE_BLADE, Form_solid)
{
}

IngredientServer * KnifeBlade::createKnifeBlade(InventoryElement * from)
{
    if (from->get_cid() == Class_Element && from->get_form() == Form_solid)
        return new KnifeBlade(from);
    else
        return nullptr;
}
