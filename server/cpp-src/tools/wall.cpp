#include "tools.h"
// #include <cstdlib>

Wall::Wall(InventoryElement * from) : IngredientServer(from, ING_WALL, Form_solid)
{
}

IngredientServer * Wall::createWall(InventoryElement * from)
{
    if ((from->get_cid() == Class_Element) || (from->get_cid() == Class_Plant))
        return new Wall(from);
    else
        return nullptr;
}
