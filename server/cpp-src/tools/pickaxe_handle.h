#ifndef PICKAXE_HANDLE_H
#define PICKAXE_HANDLE_H

#include "../elements_server.h"

class PickAxeHandle : public IngredientServer
{
  public:
    PickAxeHandle(InventoryElement * from);
};

IngredientServer* createPickAxeHandle(InventoryElement* from);
#endif
