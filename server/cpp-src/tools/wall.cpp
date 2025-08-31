#include "tools.h"
// #include <cstdlib>

Wall::Wall(InventoryElement * from) : IngredientServer(from, ING_WALL, Form_solid)
{
}

IngredientServer * Wall::createWall(InventoryElement * from)
{
    if ((from->get_base_cid() == Class_BaseElement) || (from->get_base_cid() == Class_BasePlant))
        return new Wall(from);
    else
        return nullptr;
}
