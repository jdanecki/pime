#ifndef KNIFE_HANDLE_H
#define KNIFE_HANDLE_H

#include "../elements_server.h"

class KnifeHandle : public IngredientServer
{
  public:
    KnifeHandle(InventoryElement * from);
    bool check_ing();
};
#endif
