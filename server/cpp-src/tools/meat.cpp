#include "meat.h"

Meat::Meat(InventoryElement * from) : IngredientServer(from, ING_MEAT, Form_solid)
{
}

bool Meat::check_ing()
{
    if (el->c_id == Class_Animal)
        return true;
    return false;
}
