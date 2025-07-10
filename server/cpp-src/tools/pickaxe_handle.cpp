#include "pickaxe_handle.h"
#include <cstdlib>

PickAxeHandle::PickAxeHandle(InventoryElement * from) : Ingredient(from, ING_PICKAXE_HANDLE, Form_solid)
{
}
