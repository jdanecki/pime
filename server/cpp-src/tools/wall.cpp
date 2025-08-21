#include "tools.h"
// #include <cstdlib>

Wall::Wall(InventoryElement * from) : IngredientServer(from, ING_WALL, Form_solid)
{
}

bool Wall::check_ing()
{
    if ((el->get_base_cid() == Class_BaseElement) || (el->get_base_cid() == Class_BasePlant))
        return new Wall(from);
    else
        return nullptr;
}

IngredientServer * createWall(InventoryElement * from)
{
    return new Wall(from);
}
