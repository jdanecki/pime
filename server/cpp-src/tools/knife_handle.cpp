#include "tools.h"

KnifeHandle::KnifeHandle(InventoryElement * from) : IngredientServer(from, ING_KNIFE_HANDLE, Form_solid)
{
}

IngredientServer * KnifeHandle::createKnifeHandle(InventoryElement * from)
{
    if ((from->get_cid() == Class_Element  && from->get_form() == Form_solid) ||
        from->get_cid() == Class_Plant)
        return new KnifeHandle(from);
    else
        return nullptr;
}
