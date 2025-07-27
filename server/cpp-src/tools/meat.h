#ifndef MEAT_H
#define MEAT_H

#include "../elements_server.h"

class Meat : public IngredientServer
{
  public:
    Meat(InventoryElement * from);
    bool check_ing();
    bool action(Product_action action, Player * pl);
};

IngredientServer * createMeat(InventoryElement * from);
#endif
