#ifndef AXE_H
#define AXE_H

#include "../elements_server.h"

class Axe : public ProductServer
{
  public:
    Axe(InventoryElement * el1, InventoryElement * el2);
    bool check_ing();
    // virtual bool use(int map_x, int map_y, int x, int y);
    //bool use(InventoryElement * object) override;
};

#endif
