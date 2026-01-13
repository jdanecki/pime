#include "tools.h"

Fruit::Fruit(InventoryElement * from) : IngredientServer(from, ING_FRUIT, Form_solid)
{
}

IngredientServer * Fruit::createFruit(InventoryElement * from)
{
    if (from->get_cid() == Class_Plant && dynamic_cast<Plant *>(from)->phase == Plant_fruits)
        return new Fruit(from);
    else
        return nullptr;
}
