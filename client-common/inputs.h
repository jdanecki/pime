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
extern bool finish_program;

bool handle_events();

void handle_enter();

void handle_a();
void handle_c();
void handle_i();
void handle_k();
void handle_escape();
#ifndef DISABLE_NPC
void handle_n();
#endif

void handle_f1();
void handle_f2();
void handle_f3();
void handle_f4();
void handle_f5();
void handle_f6();
void handle_f7();
void handle_f11();

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
void handle_prev_hotbar();
void handle_next_hotbar();

void handle_left();
void handle_right();
void handle_up();
void handle_down();

void handle_minus();
void handle_equal();
void handle_left_shift();
void handle_left_control();

void quit_program();

#endif
