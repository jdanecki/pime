#include "tools.h"

KnifeHandle::KnifeHandle(InventoryElement * from) : IngredientServer(from, ING_KNIFE_HANDLE, Form_solid)
{
}

IngredientServer * KnifeHandle::createKnifeHandle(InventoryElement * from)
{
    if (from->get_base_cid() == Class_BaseElement)
        return new KnifeHandle(from);
    else
        return nullptr;
}
