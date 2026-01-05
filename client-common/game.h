#ifndef GAME_H
#define GAME_H

#include "net.h"
#include "players.h"

void init_game(const char *name, int argc, char *argv[]);
void handle_network();
void put_element();

extern int auto_explore;
extern bool use_network;
extern bool show_received;

#endif // GAME_H
