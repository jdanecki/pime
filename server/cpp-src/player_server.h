#ifndef SERVER_PLAYER_H
#define SERVER_PLAYER_H

#include "../../core/player.h"

class PlayerServer : public Player
{
  public:
    void check_and_move(int new_map_x, int new_map_y, int new_x, int new_y);
    void move(int dx, int dy);
    bool use_item_on_object(InventoryElement * item, InventoryElement * object);
    bool action_on_object(Player_action a, InventoryElement * object);
    bool plant_with_seed(InventoryElement * el, int map_x, int map_y, int x, int y);
    bool pickup(InventoryElement * item);

    PlayerServer(int id);
};

#endif
