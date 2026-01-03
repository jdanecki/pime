#include "tools.h"
#include <cstdlib>

Log::Log(InventoryElement * from) : IngredientServer(from, ING_LOG, Form_solid)
{
}

IngredientServer * Log::Log::createLog(InventoryElement * from)
{
    if (from->get_cid() == Class_Plant)
        return new Log(from);
    else
        return nullptr;
}
