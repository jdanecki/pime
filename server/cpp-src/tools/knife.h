#ifndef KNIFE_H
#define KNIFE_H

#include "../elements_server.h"

class Knife : public ProductServer
{
  public:
    Knife(InventoryElement * el1, InventoryElement * el2);
    bool check_ing();
};

ProductServer * createKnife(InventoryElement * el1, InventoryElement * el2);
#endif
