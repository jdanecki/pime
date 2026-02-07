#ifndef SERVER_PLAYER_H
#define SERVER_PLAYER_H

#include "../core/player.h"
#include "elements_server.h"

class PlayerServer : public Player, public BeingServer
{
  private:
    int hunger_delay;
    int hunger_delay_max;

  public:
    void move_by(float dx, float dy);
    bool use_item_on_object(InventoryElement * item, InventoryElement * object);
    bool action_on_object(Player_action a, InventoryElement * object);
    bool server_action_on_object(Server_action a, InventoryElement * object);
    bool plant_with_seed(InventoryElement * el, int map_x, int map_y, int x, int y);
    bool pickup(InventoryElement * item);

    PlayerServer(size_t uid);
    bool use_product_on_tile(Product * prod, int map_x, int map_y, int x, int y);
    bool tick() override;
    void show(bool details = true) override;
};

PlayerServer * create_player(size_t id);
extern ElementsList * players;
void create_players();
void show_players();

#endif
