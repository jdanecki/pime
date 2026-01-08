#include <raylib.h>
#include <stdio.h>

#include "../core/tiles.h"
#include "../core/packet_types.h"

#include "playerUI.h"

#include "../dialog/d_craft.h"
#include "../dialog/d_hotbar.h"

#include "../client-common/net.h"
#include "../client-common/player_actions.h"
#include "../client-common/text.h"

extern int auto_explore;
extern int active_hotbar;

float last_key;

bool handle_events()
{
    //FIXME call update_window_size

    float dt = GetFrameTime();
    last_key+=dt;

    if (IsKeyDown(KEY_ESCAPE)) return 1;
    if (last_key < 0.1) return 0;
    last_key=0;
    bool player_moved=false;

    if (IsKeyDown(KEY_LEFT))  {
            player->going_right = 0;
            send_packet_move(client, -1, 0);
            player_moved = true;
    }
    if (IsKeyDown(KEY_RIGHT)) {
            player->going_right = 1;
            send_packet_move(client, 1, 0);
            player_moved = true;
    }
    if (IsKeyDown(KEY_UP)) {
            send_packet_move(client, 0, -1);
            player_moved = true;
    }
    if (IsKeyDown(KEY_DOWN)) {
        send_packet_move(client, 0, 1);
            player_moved = true;
    }

    if (IsKeyDown(KEY_F1)) {
       InventoryElement * item = get_item_at_ppos(player);
       if (item)
           item->show();
       else
       CONSOLE_LOG("nothing to show\n");
    }
    if (IsKeyDown(KEY_F2)) {
            server_action_tile(SERVER_SHOW_ITEM, player->location);
    }
    if (IsKeyDown(KEY_F3)) {
            show_chunk(player->location);
    }
    if (IsKeyDown(KEY_F4)) {
            server_action_tile(SERVER_SHOW_CHUNK, player->location);
    }
    if (IsKeyDown(KEY_F5)) {
            trace_network += 1;
    }
    if (IsKeyDown(KEY_F6)) {
            server_action_tile(SERVER_TRACE_NETWORK, player->location);
    }
    if (IsKeyDown(KEY_F7)) {
            auto_explore ^= 1;
    }
    if (IsKeyDown(KEY_ENTER)) {
            use_tile();
    }
    if (IsKeyDown(KEY_C)) {
        d_craft.show ^= 1;
    }

    if (IsKeyDown(KEY_ONE)) {
            active_hotbar = 0;
    }
    if (IsKeyDown(KEY_TWO)) {
            active_hotbar = 1;
    }
    if (IsKeyDown(KEY_THREE)) {
            active_hotbar = 2;
    }
    if (IsKeyDown(KEY_FOUR)) {
            active_hotbar = 3;
    }
    if (IsKeyDown(KEY_FIVE)) {
            active_hotbar = 4;
    }
    if (IsKeyDown(KEY_SIX)) {
            active_hotbar = 5;
    }
    if (IsKeyDown(KEY_SEVEN)) {
            active_hotbar = 6;
    }
    if (IsKeyDown(KEY_EIGHT)) {
            active_hotbar = 7;
    }
    if (IsKeyDown(KEY_NINE)) {
            active_hotbar = 8;
    }
    if (IsKeyDown(KEY_ZERO)) {
            active_hotbar = 9;
    }
    if (player_moved)
    {
        print_status(0, " ");
        print_status(1, " ");
    }

    return WindowShouldClose();
}

