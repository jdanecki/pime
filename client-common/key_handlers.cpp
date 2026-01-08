#include "key_handlers.h"
#include "../menu/menu.h"
#include "../dialog/d_craft.h"

bool finish_program;
void quit_program()
{
    finish_program= true;
}

void handle_i()
{
    show_menu_inventory_categories();
}

void handle_a()
{
    show_menu_action();
}

void handle_k()
{
    show_menu_knowledge();
}

void handle_escape()
{
    hide_craft_window();
    show_menu();
}

#ifndef DISABLE_NPC
void handle_n()
{
    show_menu_npc();
}
#endif

