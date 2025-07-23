#ifndef MEAT_H
#define MEAT_H

#include "../elements_server.h"

class Meat : public IngredientServer
{
  public:
    Meat(InventoryElement * from);
    bool check_ing();
};
#endif
