#include "tools.h"

PickaxeHandle::PickaxeHandle(InventoryElement * from) : IngredientServer(from, ING_PICKAXE_HANDLE, Form_solid)
{
}

IngredientServer * PickaxeHandle::createPickaxeHandle(InventoryElement * from)
{
    if ((from->get_cid() == Class_Element && from->get_form() == Form_solid) || from->get_cid() == Class_Plant)
        return new PickaxeHandle(from);
    else
        return nullptr;
}
