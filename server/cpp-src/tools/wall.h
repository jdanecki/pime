#ifndef WALL_H
#define WALL_H

#include "../elements_server.h"

class Wall : public Ingredient
{
  public:
    Wall(InventoryElement * from);
};
#endif
