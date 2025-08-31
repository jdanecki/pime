#include "tools.h"
#include <cstdlib>

Log::Log(InventoryElement * from) : IngredientServer(from, ING_LOG, Form_solid)
{
}

IngredientServer * Log::Log::createLog(InventoryElement * from)
{
    if (from->get_base_cid() == Class_BasePlant)
        return new Log(from);
    else
        return nullptr;
}
