#ifndef INPUTS_H
#define INPUTS_H

#include <stdio.h>
#include "../core/tiles.h"
#include "../core/packet_types.h"
#include "playerUI.h"
#include "../dialog/d_craft.h"
#include "../dialog/d_hotbar.h"
#include "menu.h"
#include "player_actions.h"
#include "window.h"
#include "net.h"

extern int active_hotbar;
extern int auto_explore;
extern DHotbar hotbar;

bool handle_events();

typedef void (*KeyHandlerFunc)();

struct KeyHandler {
    int key;
    KeyHandlerFunc func;
};
extern bool player_moved;

void handle_f1();
void handle_f2();
void handle_f3();
void handle_f4();
void handle_f5();
void handle_f6();
void handle_f7();
void handle_f11();

void handle_enter();
void handle_c();
void handle_hotbar_0();
void handle_hotbar_1();
void handle_hotbar_2();
void handle_hotbar_3();
void handle_hotbar_4();
void handle_hotbar_5();
void handle_hotbar_6();
void handle_hotbar_7();
void handle_hotbar_8();
void handle_hotbar_9();
void handle_left();
void handle_right();
void handle_up();
void handle_down();
void handle_prev_hotbar();
void handle_next_hotbar();
void handle_minus();
void handle_equal(); 

extern int get_num_handlers();
extern void handle_keyboard();
#endif
