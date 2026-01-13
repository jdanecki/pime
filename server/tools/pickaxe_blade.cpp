#include "tools.h"
#include <cstdlib>

PickaxeBlade::PickaxeBlade(InventoryElement * from) : IngredientServer(from, ING_PICKAXE_BLADE, Form_solid)
{
}

IngredientServer * PickaxeBlade::createPickaxeBlade(InventoryElement * from)
{
    if (from->get_cid() == Class_Element && from->get_form() == Form_solid)
        return new PickaxeBlade(from);
    else
        return nullptr;
}
