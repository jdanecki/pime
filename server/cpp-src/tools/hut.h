#ifndef HUT_H
#define HUT_H

#include "../elements_server.h"

class Hut : public ProductServer
{
  public:
    Hut(InventoryElement * el1, InventoryElement * el2);
    bool check_ing();
};

#endif
