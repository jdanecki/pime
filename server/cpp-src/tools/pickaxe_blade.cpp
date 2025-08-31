#include "tools.h"
#include <cstdlib>

PickaxeBlade::PickaxeBlade(InventoryElement * from) : IngredientServer(from, ING_PICKAXE_BLADE, Form_solid)
{
}

IngredientServer * PickaxeBlade::createPickaxeBlade(InventoryElement * from)
{
    return new PickaxeBlade(from);
}
