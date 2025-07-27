#include "knife_blade.h"

KnifeBlade::KnifeBlade(InventoryElement * from) : IngredientServer(from, ING_KNIFE_BLADE, Form_solid)
{
}

bool KnifeBlade::check_ing()
{
    if (el->get_base_cid() == Class_BaseElement)
        return true;
    else
        return false;
}

IngredientServer * createKnifeBlade(InventoryElement * from)
{
    return new KnifeBlade(from);
}
