#ifndef KNIFE_H
#define KNIFE_H

#include "../elements_server.h"

class Knife : public Product
{
  public:
    Knife(InventoryElement * el1, InventoryElement * el2);
   // virtual bool use(int map_x, int map_y, int x, int y);
    bool check_ing();
};
#endif
