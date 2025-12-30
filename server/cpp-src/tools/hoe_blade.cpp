#include "tools.h"
#include <cstdlib>

HoeBlade::HoeBlade(InventoryElement * from) : IngredientServer(from, ING_HOE_BLADE, Form_solid)
{
}

IngredientServer * HoeBlade::createHoeBlade(InventoryElement * from)
{
    if (from->get_cid() == Class_Element && from->get_form() == Form_solid)
        return new HoeBlade(from);
    else return nullptr;
}
