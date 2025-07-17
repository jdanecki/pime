#ifndef MAIN_H
#define MAIN_H

#include "../core/world.h"
#include "implementations/alchemistSDL.h"
#include "npc.h"
#include "text.h"
#include "window.h"
#include <SDL2/SDL.h>
#include <cstdlib>
#include <time.h>
// #include <SDL2/SDL_render.h>
#include <stdio.h>
#include <stdlib.h>
// #include <SDL2/SDL2_framerate.h>
#include "../core/tiles.h"
#include "dialog/d_craft.h"
#include "dialog/d_hotbar.h"
#include "inputs.h"
#include "menu.h"
#include "networking.h"
#include <sys/stat.h>
#include <sys/types.h>
#include <termios.h>
#include <unistd.h>

extern Player * player;
#define PLAYER_NUM 16
extern Player * players[PLAYER_NUM];
extern SDL_Texture * map;
#endif // MAIN_H
