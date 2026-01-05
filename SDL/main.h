#ifndef MAIN_H
#define MAIN_H

#include <SDL2/SDL.h>
#include <cstdlib>
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <termios.h>
#include <unistd.h>

#include "../core/world.h"
#include "../core/tiles.h"

#include "npc.h"
#include "alchemistSDL.h"
#include "BeingSDL.h"
#include "text.h"
#include "window.h"
// #include <SDL2/SDL_render.h>
// #include <SDL2/SDL2_framerate.h>
#include "dialog/d_craft.h"
#include "dialog/d_hotbar.h"
#include "inputs.h"
#include "menu.h"
#include "../client-common/game.h"

extern SDL_Texture * map;
#endif // MAIN_H
