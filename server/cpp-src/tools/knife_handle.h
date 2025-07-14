#ifndef KNIFE_HANDLE_H
#define KNIFE_HANDLE_H

#include "../elements_server.h"

class KnifeHandle : public IngredientServer
{
  public:
    KnifeHandle(InventoryElement * from);
};
#endif
