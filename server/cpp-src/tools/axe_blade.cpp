#include "axe_blade.h"
#include <cstdlib>

AxeBlade::AxeBlade(InventoryElement * from) : IngredientServer(from, ING_AXE_BLADE, Form_solid)
{
}

bool AxeBlade::check_ing()
{
    if (el->get_base_cid() == Class_BaseElement)
        return true;
    else return false;
}

IngredientServer *createAxeBlade(InventoryElement *from)
{
    return new AxeBlade(from);
}
