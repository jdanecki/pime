#include "tools.h"
#include <cstdlib>

PickAxeHandle::PickAxeHandle(InventoryElement * from) : IngredientServer(from, ING_PICKAXE_HANDLE, Form_solid)
{
}

IngredientServer * createPickAxeHandle(InventoryElement * from)
{
    return new PickAxeHandle(from);
}
