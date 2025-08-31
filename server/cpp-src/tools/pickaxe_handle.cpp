#include "tools.h"

PickaxeHandle::PickaxeHandle(InventoryElement * from) : IngredientServer(from, ING_PICKAXE_HANDLE, Form_solid)
{
}

IngredientServer * PickaxeHandle::createPickaxeHandle(InventoryElement * from)
{
    return new PickaxeHandle(from);
}
