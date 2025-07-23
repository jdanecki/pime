#include "axe_handle.h"
#include <cstdlib>

AxeHandle::AxeHandle(InventoryElement * from) : IngredientServer(from, ING_AXE_HANDLE, Form_solid)
{
}

bool AxeHandle::check_ing()
{
    if (el->get_base_cid() == Class_BaseElement)
        return true;
    else return false;
}

IngredientServer *createAxeHandle(InventoryElement *from)
{
    return new AxeHandle(from);
}
