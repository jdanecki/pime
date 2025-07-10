#ifndef HUT_H
#define HUT_H

#include "../elements_server.h"

class Hut : public Product
{
  public:
    Hut(InventoryElement * el1, InventoryElement * el2);
    bool check_ing();
    // virtual bool use(int map_x, int map_y, int x, int y);

    bool use(InventoryElement * object) override
    {
        printf("what do you want to do with %s on %s, destroy?\n", object->get_name(), get_name());
        return false;
    }
};

#endif
