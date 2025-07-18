#ifndef PICKAXE_H
#define PICKAXE_H

#include "../elements_server.h"

class PickAxe : public ProductServer
{
  public:
    PickAxe(InventoryElement * el1, InventoryElement * el2);
    bool check_ing();
};

#endif
