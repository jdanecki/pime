#ifndef FRUIT_SALAD_H
#define FRUIT_SALAD_H

#include "../elements_server.h"

class Fruit_salad : public ProductServer
{
  public:
    Fruit_salad(InventoryElement * el1, InventoryElement * el2);
    bool check_ing();
    bool player_action(Player_action action, Player * pl);
};

ProductServer * createFruit_salad(InventoryElement * el1, InventoryElement * el2);

#endif
