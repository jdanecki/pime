#ifndef FIRE_H
#define FIRE_H

#include "../elements_server.h"

class Fire : public ProductServer
{
  public:
    Fire(InventoryElement * el1, InventoryElement * el2);
    bool check_ing();
};

#endif
