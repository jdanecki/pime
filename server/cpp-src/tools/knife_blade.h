#ifndef KNIFE_BLADE_H
#define KNIFE_BLADE_H

#include "../elements_server.h"

class KnifeBlade : public IngredientServer
{
  public:
    KnifeBlade(InventoryElement * from);
    bool check_ing();
};

IngredientServer* createKnifeBlade(InventoryElement* from);
#endif
