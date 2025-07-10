#include "wall.h"
#include <cstdlib>

Wall::Wall(InventoryElement * from) : Ingredient(from, ING_WALL, Form_solid)
{
}
