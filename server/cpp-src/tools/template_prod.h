#ifndef TEMPLATE_H
#define TEMPLATE_H

#include "../elements_server.h"

class CLASS_NAME : public ProductServer
{
  public:
    CLASS_NAME(InventoryElement * el1, InventoryElement * el2);
    bool check_ing();
};

ProductServer * createCLASS_NAME(InventoryElement * el1, InventoryElement * el2);

#endif
