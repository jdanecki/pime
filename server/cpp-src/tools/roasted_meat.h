#ifndef ROASTED_MEAT_H
#define ROASTED_MEAT_H

#include "../elements_server.h"

class Roasted_meat : public ProductServer
{
  public:
    Roasted_meat(InventoryElement * el1, InventoryElement * el2);
    bool check_ing();
};

#endif
