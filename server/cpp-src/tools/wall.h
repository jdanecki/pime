#ifndef WALL_H
#define WALL_H

#include "../elements_server.h"

class Wall : public IngredientServer
{
  public:
    Wall(InventoryElement * from);
    bool check_ing();
};
IngredientServer * createWall(InventoryElement * from);
#endif
