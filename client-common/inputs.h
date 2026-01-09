#ifndef INPUTS_H
#define INPUTS_H

#include <stdio.h>
#include "../core/tiles.h"
#include "../core/packet_types.h"
#include "playerUI.h"
#include "../dialog/d_craft.h"
#include "../dialog/d_hotbar.h"
#include "../menu/menu.h"
#include "player_actions.h"
#include "window.h"
#include "net.h"
#include "key_handler.h"
#include "key_handlers.h"

extern int active_hotbar;
extern int auto_explore;
extern DHotbar hotbar;

bool handle_events();

extern bool player_moved;

#endif
