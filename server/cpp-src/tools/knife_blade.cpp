#include "knife_blade.h"

KnifeBlade::KnifeBlade(InventoryElement * from) : IngredientServer(from, ING_KNIFE_BLADE, Form_solid)
{
}

bool KnifeBlade::check_ing()
{
    return true;
}
