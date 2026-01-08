#include "inputs.h"

bool player_moved;

void handle_f1() {
    InventoryElement * item = get_item_at_ppos(player);
    if (item)
        item->show();
    else
        CONSOLE_LOG("nothing to show\n");
}
void handle_f2() { server_action_tile(SERVER_SHOW_ITEM, player->location); }
void handle_f3() { show_chunk(player->location); }
void handle_f4() { server_action_tile(SERVER_SHOW_CHUNK, player->location); }
void handle_f5() { trace_network += 1; }
void handle_f6() { server_action_tile(SERVER_TRACE_NETWORK, player->location); }
void handle_f7() { auto_explore ^= 1; }
//void handle_f11() { update_window_size(); }

void handle_enter() { use_tile(); }
void handle_c() { d_craft.show = 1; }
void handle_hotbar_0() { active_hotbar = 0; }
void handle_hotbar_1() { active_hotbar = 1; }
void handle_hotbar_2() { active_hotbar = 2; }
void handle_hotbar_3() { active_hotbar = 3; }
void handle_hotbar_4() { active_hotbar = 4; }
void handle_hotbar_5() { active_hotbar = 5; }
void handle_hotbar_6() { active_hotbar = 6; }
void handle_hotbar_7() { active_hotbar = 7; }
void handle_hotbar_8() { active_hotbar = 8; }
void handle_hotbar_9() { active_hotbar = 9; }
void handle_q() { put_element(); }

void handle_prev_hotbar() {
    active_hotbar--;
    if (active_hotbar == -1)
        active_hotbar = 9;
}
void handle_next_hotbar() {
    active_hotbar++;
    if (active_hotbar == 10)
        active_hotbar = 0;
}

void handle_minus() {
    player->craftbar[active_hotbar] = 0;
}
void handle_equal() {
    if (player->hotbar[active_hotbar])
        player->craftbar[active_hotbar] = 1;
}  

void handle_left() {
    player->going_right = 0;
    send_packet_move(client, -1, 0);    
}
void handle_right() {
    player->going_right = 1;
    send_packet_move(client, 1, 0);    
}
void handle_up() {
    send_packet_move(client, 0, -1);

}
void handle_down() {
    send_packet_move(client, 0, 1);

}

