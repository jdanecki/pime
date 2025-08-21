#include "tools.h"

Fruit::Fruit(InventoryElement * from) : IngredientServer(from, ING_FRUIT, Form_solid)
{
}

bool Fruit::check_ing()
{
    if (el->get_base_cid() == Class_BasePlant && dynamic_cast<Plant *>(el)->phase == Plant_fruits)
        return new Fruit(from);
    else
        return nullptr;
}

IngredientServer * Fruit::createFruit(InventoryElement * from)
{
    return new Fruit(from);
}
