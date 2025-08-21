#include "tools.h"
#include <cstdlib>

AxeHandle::AxeHandle(InventoryElement * from) : IngredientServer(from, ING_AXE_HANDLE, Form_solid)
{
}

IngredientServer * AxeHandle::AxeHandle::createAxeHandle(InventoryElement * from)
{
    if (from->get_base_cid() == Class_BaseElement)
        return new AxeHandle(from);
    else
        return nullptr;
}
