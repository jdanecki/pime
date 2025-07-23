#ifndef PICKAXE_BLADE_H
#define PICKAXE_BLADE_H

#include "../elements_server.h"

class PickAxeBlade : public IngredientServer
{
  public:
    PickAxeBlade(InventoryElement * from);
};

IngredientServer* createPickAxeBlade(InventoryElement* from);
#endif
