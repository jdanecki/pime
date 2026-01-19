#include "inputs.h"
#include "../core/alchemist/elements.h"
#include "playerUI.h"
#include "player_actions.h"
#include "../core/packet_types.h"
#include "../menu/menu.h"
#include "../dialog/d_craft.h"

bool finish_program;

void handle_show_item()
{
    InventoryElement * item = get_item_at_ppos(player);
    if (item)
        item->show();
    else
        CONSOLE_LOG("nothing to show\n");
}
void handle_show_item_server()
{
    server_action_tile(SERVER_SHOW_ITEM, player->location);
}
void handle_show_chunk()
{
    show_chunk(player->location);
}
void handle_show_chunk_server()
{
    server_action_tile(SERVER_SHOW_CHUNK, player->location);
}
void handle_trace_network()
{
    trace_network += 1;
}
void handle_trace_network_server()
{
    server_action_tile(SERVER_TRACE_NETWORK, player->location);
}
void handle_auto_explore()
{
    auto_explore ^= 1;
}
// void handle_f11() { update_window_size(); }

void handle_use_tile()
{
    use_tile();
}
void handle_pickup_item()
{
    pickup_item();
}
void handle_put_item()
{
    put_item();
}

void handle_craft_show()
{
    d_craft.show = 1;
}
void handle_hotbar_0()
{
    active_hotbar = 0;
}
void handle_hotbar_1()
{
    active_hotbar = 1;
}
void handle_hotbar_2()
{
    active_hotbar = 2;
}
void handle_hotbar_3()
{
    active_hotbar = 3;
}
void handle_hotbar_4()
{
    active_hotbar = 4;
}
void handle_hotbar_5()
{
    active_hotbar = 5;
}
void handle_hotbar_6()
{
    active_hotbar = 6;
}
void handle_hotbar_7()
{
    active_hotbar = 7;
}
void handle_hotbar_8()
{
    active_hotbar = 8;
}
void handle_hotbar_9()
{
    active_hotbar = 9;
}

void handle_prev_hotbar()
{
    active_hotbar--;
    if (active_hotbar == -1)
        active_hotbar = 9;
}
void handle_next_hotbar()
{
    active_hotbar++;
    if (active_hotbar == 10)
        active_hotbar = 0;
}

void handle_craftbar_prev()
{
    player->craftbar[active_hotbar] = 0;
}
void handle_craftbar_next()
{
    if (player->hotbar[active_hotbar])
        player->craftbar[active_hotbar] = 1;
}

void handle_left_shift()
{
    player->sneaking = 1;
}

void handle_left_control()
{
    player->running = 1;
}

void quit_program()
{
    finish_program = true;
}

void handle_inventory()
{
    show_menu_inventory_categories();
}

void handle_menu_action()
{
    show_menu_action();
}

void handle_menu_knowledge()
{
    show_menu_knowledge();
}

void handle_escape()
{
    if (!hide_craft_window())
        show_menu();
}

#ifndef DISABLE_NPC
void handle_menu_npc()
{
    show_menu_npc();
}
#endif
