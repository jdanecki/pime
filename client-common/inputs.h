#ifndef INPUTS_H
#define INPUTS_H

#include "../dialog/d_hotbar.h"

extern int active_hotbar;
extern int auto_explore;
extern DHotbar hotbar;
extern bool finish_program;

bool handle_events();

void handle_use_tile();
void handle_pickup_item();
void handle_put_item();

void handle_craft_show();
void handle_inventory();
void handle_menu_action();
void handle_menu_knowledge();
void handle_escape();

#ifndef DISABLE_NPC
void handle_menu_npc();
#endif

void handle_show_item();
void handle_show_item_server();
void handle_show_chunk();
void handle_show_chunk_server();
void handle_trace_network();
void handle_trace_network_server();
void handle_auto_explore();

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

void handle_craftbar_prev();
void handle_craftbar_next();
void handle_left_shift();
void handle_left_control();

void quit_program();

#endif
