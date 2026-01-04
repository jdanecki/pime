#ifndef MAIN_H
#define MAIN_H

#include "../core/world.h"
#include "implementations/playerSDL.h"
#include <cstdlib>
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include "../core/tiles.h"
#include "../core/networking.h"
#include <sys/stat.h>
#include <sys/types.h>
#include <termios.h>
#include <unistd.h>

extern PlayerSDL * player;
#define PLAYER_NUM 16
extern PlayerSDL * players[PLAYER_NUM];
#endif // MAIN_H
