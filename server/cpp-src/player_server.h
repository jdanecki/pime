#ifndef SERVER_PLAYER_H
#define SERVER_PLAYER_H

#include "../../core/player.h"
#include "networking.h"
#include "../../core/npc.h"

class PlayerServer : public Player
{
  public:
    void move(int dx, int dy);
    bool use_item_on_object(InventoryElement * item, InventoryElement * object);
    bool action_on_object(Player_action a, InventoryElement * object);
    bool server_action_on_object(Server_action a, InventoryElement * object);
    bool plant_with_seed(InventoryElement * el, int map_x, int map_y, int x, int y);
    bool pickup(InventoryElement * item);

    PlayerServer(size_t id);
};

PlayerServer * create_player(size_t id);

Npc * create_npc();

#endif
