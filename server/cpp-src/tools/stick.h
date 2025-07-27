#ifndef STICK_H
#define STICK_H

#include "../elements_server.h"

class Stick : public IngredientServer
{
  public:
    Stick(InventoryElement * from);
    bool check_ing();
};

IngredientServer * createStick(InventoryElement * from);
#endif
