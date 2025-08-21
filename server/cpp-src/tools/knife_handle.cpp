#include "tools.h"

KnifeHandle::KnifeHandle(InventoryElement * from) : IngredientServer(from, ING_KNIFE_HANDLE, Form_solid)
{
}

bool KnifeHandle::check_ing()
{
    if (el->get_base_cid() == Class_BaseElement)
        return new KnifeHandle(from);
    else
        return nullptr;
}

IngredientServer * createKnifeHandle(InventoryElement * from)
{
    return new KnifeHandle(from);
}
