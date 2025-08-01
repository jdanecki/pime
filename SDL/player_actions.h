#ifndef PLAYER_ACTIONS_H
#define PLAYER_ACTIONS_H
#include "../core/player.h"

void put_element();
void use_tile(int map_x, int map_y, int x, int y);
void action_tile(Player_action a, int map_x, int map_y, int x, int y);
void server_action_tile(Server_action a, int map_x, int map_y, int x, int y);
#endif
