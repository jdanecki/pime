#ifndef GAME_H
#define GAME_H

#include "net.h"

void init_game(const char *name, int argc, char *argv[]);
void handle_network();
extern int auto_explore;
extern bool use_network;
extern bool show_received;

#endif // GAME_H
