#include "pickaxe_blade.h"
#include <cstdlib>

PickAxeBlade::PickAxeBlade(InventoryElement * from) : IngredientServer(from, ING_PICKAXE_BLADE, Form_solid)
{
}
