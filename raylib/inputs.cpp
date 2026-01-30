#include <raylib.h>

#include "../core/key_handler.h"
#include "../client-common/2d/inputs.h"
#include "../client-common/ui/menu/menu.h"
#include "../client-common/ui/dialog/d_craft.h"
#include "../net/net.h"

#include "playerUI.h"

KeyHandler menu_key_handlers[] = {
    {KEY_ESCAPE, menu_handle_escape, true},
    {KEY_ENTER, menu_handle_enter, true},
    {KEY_DOWN, menu_go_down, false},
    {KEY_UP, menu_go_up, false},
};

KeyHandler key_handlers[] = {
    {KEY_F1, handle_show_item, true},
    {KEY_F2, handle_show_item_server, true},
    {KEY_F3, handle_show_chunk, true},
    {KEY_F4, handle_show_chunk_server, true},
    {KEY_F5, handle_trace_network, true},
    {KEY_F6, handle_trace_network_server, true},
    {KEY_F7, handle_auto_explore, true},
    {KEY_ENTER, handle_use_tile, true},
    {KEY_E, handle_pickup_item, true},
    {KEY_ONE, handle_hotbar_0, true},
    {KEY_TWO, handle_hotbar_1, true},
    {KEY_THREE, handle_hotbar_2, true},
    {KEY_FOUR, handle_hotbar_3, true},
    {KEY_FIVE, handle_hotbar_4, true},
    {KEY_SIX, handle_hotbar_5, true},
    {KEY_SEVEN, handle_hotbar_6, true},
    {KEY_EIGHT, handle_hotbar_7, true},
    {KEY_NINE, handle_hotbar_8, true},
    {KEY_ZERO, handle_hotbar_9, true},
    {KEY_C, handle_craft_show, true},
    {KEY_I, handle_inventory, true},
    {KEY_Z, handle_menu_action, true},
    {KEY_K, handle_menu_knowledge, true},
    {KEY_N, handle_menu_npc, true},
    {KEY_Q, handle_put_item, true},
    {KEY_GRAVE, handle_prev_hotbar, false},
    {KEY_TAB, handle_next_hotbar, false},
    {KEY_MINUS, handle_craftbar_prev, true},
    {KEY_EQUAL, handle_craftbar_next, true},
    {KEY_ESCAPE, handle_escape, true},
    {KEY_LEFT_CONTROL, handle_left_control, true},
    {KEY_LEFT_SHIFT, handle_left_shift, true},
};

void mouse_pressed(int x, int y, int button)
{
    CONSOLE_LOG("mouse %d,%d %d \n", x, y, button);
    hotbar.press(x, y, button);
    if (d_craft.show)
    {
        d_craft.press(x, y, button);
    }
}

void handle_mouse()
{
    if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
    {
        Vector2 pos = GetMousePosition();
        mouse_pressed((int)pos.x, (int)pos.y, 1);
    }
    if (IsMouseButtonPressed(MOUSE_MIDDLE_BUTTON))
    {
        Vector2 pos = GetMousePosition();
        mouse_pressed((int)pos.x, (int)pos.y, 2);
    }
    if (IsMouseButtonPressed(MOUSE_RIGHT_BUTTON))
    {
        Vector2 pos = GetMousePosition();
        mouse_pressed((int)pos.x, (int)pos.y, 3);
    }
}

float last_key;
float last_move;

bool handle_events()
{
    // FIXME call update_window_size

    handle_mouse();

    float speed = 0.2;

    if (player->sneaking)
    {
        speed = 0.1;
    }
    if (player->running)
    {
        speed = 0.4;
    }

    int num_handlers;
    KeyHandler * handlers;

    last_key += GetFrameTime();
    last_move += GetFrameTime();

    if (current_menu)
    {
        num_handlers = sizeof(menu_key_handlers) / sizeof(KeyHandler);
        handlers = menu_key_handlers;
    }
    else
    {
        num_handlers = sizeof(key_handlers) / sizeof(KeyHandler);
        handlers = key_handlers;
    }
    if (IsKeyReleased(KEY_LEFT_CONTROL))
        player->running = 0;
    if (IsKeyReleased(KEY_LEFT_SHIFT))
        player->sneaking = 0;

    for (int i = 0; i < num_handlers; ++i)
    {
        if (handlers[i].press_key)
        {
            if (IsKeyPressed(handlers[i].key))
            {
                handlers[i].func();
                break;
            }
        }
        else
        {
            if (IsKeyDown(handlers[i].key))
            {
                if (last_key < 0.1)
                    break;
                last_key = 0;
                handlers[i].func();
                break;
            }
        }
    }

    if (!current_menu)
    {
        //   if (last_move > 0.1)
        {
            if (IsKeyDown(KEY_DOWN) || IsKeyDown(KEY_S))
            {
                send_packet_move(0, speed);
                last_move = 0;
            }
            if (IsKeyDown(KEY_UP) || IsKeyDown(KEY_W))
            {
                send_packet_move(0, -speed);
                last_move = 0;
            }
            if (IsKeyDown(KEY_RIGHT) || IsKeyDown(KEY_D))
            {
                player->going_right = 1;
                send_packet_move(speed, 0);
                last_move = 0;
            }
            if (IsKeyDown(KEY_LEFT) || IsKeyDown(KEY_A))
            {
                player->going_right = 0;
                send_packet_move(-speed, 0);
                last_move = 0;
            }
        }
    }
    return finish_program || WindowShouldClose();
}
