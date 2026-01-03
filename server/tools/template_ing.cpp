#include "tools.h"

CLASS_NAME::CLASS_NAME(InventoryElement * from) : IngredientServer(from, ING, Form_solid)
{
}

IngredientServer * CLASS_NAME::createCLASS_NAME(InventoryElement * from)
{
    return new CLASS_NAME(from);
}
