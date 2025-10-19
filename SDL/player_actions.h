#ifndef PLAYER_ACTIONS_H
#define PLAYER_ACTIONS_H
#include "../core/player.h"

void put_element();
void use_tile();
void action_tile(Player_action a, ItemLocation loc);
void server_action_tile(Server_action a, ItemLocation loc);
#endif
