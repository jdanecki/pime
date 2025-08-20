#include "tools.h"
#include <cstdlib>

AxeBlade::AxeBlade(InventoryElement * from) : IngredientServer(from, ING_AXE_BLADE, Form_solid)
{
}

IngredientServer * AxeBlade::createAxeBlade(InventoryElement * from)
{
    if (from->get_base_cid() == Class_BaseElement)
        return new AxeBlade(from);
    else
        return nullptr;
}
