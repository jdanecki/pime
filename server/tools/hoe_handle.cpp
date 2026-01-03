#include "tools.h"

HoeHandle::HoeHandle(InventoryElement * from) : IngredientServer(from, ING_HOE_HANDLE, Form_solid)
{
}

IngredientServer * HoeHandle::createHoeHandle(InventoryElement * from)
{
    if ((from->get_cid() == Class_Element  && from->get_form() == Form_solid)
        || from->get_cid() == Class_Plant)
        return new HoeHandle(from);
    else return nullptr;
}
