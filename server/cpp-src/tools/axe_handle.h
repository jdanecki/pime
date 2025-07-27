#ifndef AXE_HANDLE_H
#define AXE_HANDLE_H

#include "../elements_server.h"

class AxeHandle : public IngredientServer
{
  public:
    AxeHandle(InventoryElement * from);
    bool check_ing();
};
IngredientServer * createAxeHandle(InventoryElement * from);
#endif
