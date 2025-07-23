#include "knife_handle.h"

KnifeHandle::KnifeHandle(InventoryElement * from) : IngredientServer(from, ING_KNIFE_HANDLE, Form_solid)
{
}

bool KnifeHandle::check_ing()
{
    return true;
}
