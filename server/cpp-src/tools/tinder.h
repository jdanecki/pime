#ifndef TINDER_H
#define TINDER_H

#include "../elements_server.h"

class Tinder : public IngredientServer
{
  public:
    Tinder(InventoryElement * from);
    bool check_ing();
};
#endif
