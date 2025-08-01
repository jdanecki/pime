#ifndef FRUIT_H
#define FRUIT_H

#include "../elements_server.h"

class Fruit : public IngredientServer
{
  public:
    Fruit(InventoryElement * from);
    bool check_ing();
};

IngredientServer * createFruit(InventoryElement * from);
#endif
