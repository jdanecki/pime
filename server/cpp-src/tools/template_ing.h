#ifndef TEMPLATE_H
#define TEMPLATE_H

#include "../elements_server.h"

class CLASS_NAME : public IngredientServer
{
  public:
    CLASS_NAME(InventoryElement * from);
    bool check_ing();
};
#endif
