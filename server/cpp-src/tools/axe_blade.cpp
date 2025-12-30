#include "tools.h"
#include <cstdlib>

AxeBlade::AxeBlade(InventoryElement * from) : IngredientServer(from, ING_AXE_BLADE, Form_solid)
{
    printf("AXE constr\n");
}

IngredientServer * AxeBlade::AxeBlade::createAxeBlade(InventoryElement * from)
{
    if (from->get_cid() == Class_Element && from->get_form() == Form_solid)
        return new AxeBlade(from);
    else
        return nullptr;
}
