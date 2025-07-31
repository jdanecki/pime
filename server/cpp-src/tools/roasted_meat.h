#ifndef ROASTED_MEAT_H
#define ROASTED_MEAT_H

#include "../elements_server.h"

class Roasted_meat : public ProductServer
{
  public:
    Roasted_meat(InventoryElement * el1, InventoryElement * el2);
    bool player_action(Player_action action, Player * pl) override;
    bool check_ing();
};

ProductServer * createRoasted_meat(InventoryElement * el1, InventoryElement * el2);

#endif
