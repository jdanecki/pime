#include "tools.h"
#include <cstdlib>

AxeHandle::AxeHandle(InventoryElement * from) : IngredientServer(from, ING_AXE_HANDLE, Form_solid)
{
}

IngredientServer * AxeHandle::createAxeHandle(InventoryElement * from)
{
    if ((from->get_cid() == Class_Element && from->get_form() == Form_solid) ||
        from->get_cid() == Class_Plant)
        return new AxeHandle(from);
    else
        return nullptr;
}
