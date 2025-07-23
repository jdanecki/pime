#include "axe_handle.h"
#include <cstdlib>

AxeHandle::AxeHandle(InventoryElement * from) : IngredientServer(from, ING_AXE_HANDLE, Form_solid)
{
}

bool AxeHandle::check_ing()
{
    return true;
}
