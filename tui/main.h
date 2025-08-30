#ifndef MAIN_H
#define MAIN_H

#include "../core/world.h"
#include "implementations/alchemistSDL.h"
#include <cstdlib>
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include "../core/tiles.h"
#include "../SDL/networking.h"
#include <sys/stat.h>
#include <sys/types.h>
#include <termios.h>
#include <unistd.h>

extern Player * player;
#define PLAYER_NUM 16
extern Player * players[PLAYER_NUM];
#endif // MAIN_H
