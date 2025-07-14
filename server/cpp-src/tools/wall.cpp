#include "wall.h"
#include <cstdlib>

Wall::Wall(InventoryElement * from) : IngredientServer(from, ING_WALL, Form_solid)
{
}
