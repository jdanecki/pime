#include "axe_blade.h"
#include <cstdlib>

AxeBlade::AxeBlade(InventoryElement * from) : IngredientServer(from, ING_AXE_BLADE, Form_solid)
{
}

bool AxeBlade::check_ing()
{
    return true;
}
