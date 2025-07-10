#include "pickaxe_blade.h"
#include <cstdlib>

PickAxeBlade::PickAxeBlade(InventoryElement * from) : Ingredient(from, ING_PICKAXE_BLADE, Form_solid)
{
}
