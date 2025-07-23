#include "log.h"
#include <cstdlib>

Log::Log(InventoryElement * from) : IngredientServer(from, ING_LOG, Form_solid)
{
}

bool Log::check_ing()
{
    if (el->get_base_cid() == Class_BasePlant)
        return true;
    else return false;
}
