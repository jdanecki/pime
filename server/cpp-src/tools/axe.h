#ifndef AXE_H
#define AXE_H

#include "../elements_server.h"

class Axe : public ProductServer
{
  public:
    Axe(InventoryElement * el1, InventoryElement * el2);
    bool check_ing();
};

ProductServer* createAxe(InventoryElement* el1, InventoryElement* el2);

#endif
