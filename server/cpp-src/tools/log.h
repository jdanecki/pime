#ifndef LOG_H
#define LOG_H

#include "../elements_server.h"

class Log : public IngredientServer
{
  public:
    Log(InventoryElement * from);
    bool check_ing();
};
#endif
