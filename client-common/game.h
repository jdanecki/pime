#ifndef GAME_H
#define GAME_H

#include "net.h"
#include "players.h"

extern void start_game(const char *name, int argc, char *argv[]);
extern void handle_network();
extern void put_element();

extern int auto_explore;
extern bool use_network;
extern bool show_received;

#endif // GAME_H
