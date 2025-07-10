#ifndef PICKAXE_H
#define PICKAXE_H

#include "../elements_server.h"

class PickAxe : public Product
{
  public:
    PickAxe(InventoryElement * el1, InventoryElement * el2);
    bool check_ing();
    // virtual bool use(int map_x, int map_y, int x, int y);
    //bool use(InventoryElement * object) override;
};

#endif
