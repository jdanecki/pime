#include "tools.h"

KnifeBlade::KnifeBlade(InventoryElement * from) : IngredientServer(from, ING_KNIFE_BLADE, Form_solid)
{
}

IngredientServer * KnifeBlade::createKnifeBlade(InventoryElement * from)
{
    if (from->get_base_cid() == Class_BaseElement)
        return new KnifeBlade(from);
    else
        return nullptr;
}
