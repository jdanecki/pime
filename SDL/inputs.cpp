#include <SDL2/SDL.h>
#include <SDL2/SDL_keyboard.h>
#include <SDL2/SDL_keycode.h>
#include <SDL2/SDL_mouse.h>
#include <SDL2/SDL_scancode.h>
#include <SDL2/SDL_timer.h>

#include "../core/key_handler.h"

#include "../net/net.h"

#include "../client-common/2d/inputs.h"
#include "../client-common/ui/window.h"
#include "../client-common/ui/menu/menu.h"
#include "../client-common/ui/dialog/d_craft.h"

#include "playerUI.h"

KeyHandler menu_key_handlers[] = {
    {SDLK_ESCAPE, menu_handle_escape, true},
    {SDLK_RETURN, menu_handle_enter, true},
    {SDLK_DOWN, menu_go_down, false},
    {SDLK_UP, menu_go_up, false},
};

KeyHandler key_handlers[] = {
    {SDLK_F1, handle_show_item, true},
    {SDLK_F2, handle_show_item_server, true},
    {SDLK_F3, handle_show_chunk, true},
    {SDLK_F4, handle_show_chunk_server, true},
    {SDLK_F5, handle_trace_network, true},
    {SDLK_F6, handle_trace_network_server, true},
    {SDLK_F7, handle_auto_explore, true},
    {SDLK_RETURN, handle_use_tile, true},
    {SDLK_e, handle_pickup_item, true},
    {SDLK_1, handle_hotbar_0, true},
    {SDLK_2, handle_hotbar_1, true},
    {SDLK_3, handle_hotbar_2, true},
    {SDLK_4, handle_hotbar_3, true},
    {SDLK_5, handle_hotbar_4, true},
    {SDLK_6, handle_hotbar_5, true},
    {SDLK_7, handle_hotbar_6, true},
    {SDLK_8, handle_hotbar_7, true},
    {SDLK_9, handle_hotbar_8, true},
    {SDLK_0, handle_hotbar_9, true},
    {SDLK_c, handle_craft_show, true},
    {SDLK_i, handle_inventory, true},
    {SDLK_z, handle_menu_action, true},
    {SDLK_k, handle_menu_knowledge, true},
    {SDLK_n, handle_menu_npc, true},
    {SDLK_q, handle_put_item, true},

    {SDLK_BACKQUOTE, handle_prev_hotbar, false},
    {SDLK_TAB, handle_next_hotbar, false},
    {SDLK_MINUS, handle_craftbar_prev, true},
    {SDLK_EQUALS, handle_craftbar_next, true},
    {SDLK_ESCAPE, handle_escape, true},
};

bool key_pressed(int key)
{
    int num_handlers;
    KeyHandler * handlers;

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
    for (int i = 0; i < num_handlers; ++i)
    {
        if (key == handlers[i].key)
        {
            handlers[i].func();
        }
    }
    return finish_program;
}

void mouse_pressed(SDL_MouseButtonEvent * event)
{
    CONSOLE_LOG("mouse %d,%d %d \n", event->x, event->y, event->button);
    hotbar.press(event->x, event->y, event->button);
    if (d_craft.show)
    {
        d_craft.press(event->x, event->y, event->button);
    }
}

extern int tile_size;

void mouse_wheel(SDL_MouseWheelEvent * event)
{
    printf("mouse wheel: %d, %d \n", event->x, event->y);
    tile_size += event->y;
    if (tile_size < 10)
        tile_size = 10;
    CONSOLE_LOG("tile_size=%d\n", tile_size);
}

void handle_keyboard_state(const Uint8 * keys)
{
    float speed = 0.2;
    if (keys[SDL_SCANCODE_LSHIFT])
    {
        handle_left_shift();
        speed = 0.1;
    }
    else
    {
        player->sneaking = 0;
        if (keys[SDL_SCANCODE_LCTRL] && player->hunger && player->thirst)
        {
            handle_left_control();
            speed = 0.4;
        }
        else
        {
            player->running = 0;
        }
    }

    if (keys[SDL_SCANCODE_DOWN] || keys[SDL_SCANCODE_S])
    {
        send_packet_move(0, speed);
    }
    else if (keys[SDL_SCANCODE_UP] || keys[SDL_SCANCODE_W])
    {
        send_packet_move(0, -speed);
    }
    if (keys[SDL_SCANCODE_RIGHT] || keys[SDL_SCANCODE_D])
    {
        player->going_right = 1;
        send_packet_move(speed, 0);
    }
    else if (keys[SDL_SCANCODE_LEFT] || keys[SDL_SCANCODE_A])
    {
        player->going_right = 0;
        send_packet_move(-speed, 0);
    }
}

unsigned int last_move;

bool handle_events()
{
    int ww = 0, wh = 0;
    SDL_Event event;
    bool ret = false;

    while (SDL_PollEvent(&event))
    {
        if (event.type == SDL_QUIT)
        {
            return true;
        };
        if (event.type == SDL_KEYDOWN)
        {
            int key = event.key.keysym.sym;
            //   printf("key %d\n", key);

            ret = key_pressed(key);
        }
        if (event.type == SDL_WINDOWEVENT && event.window.event == SDL_WINDOWEVENT_RESIZED)
        {
            // i3 window manager sends these events if window is not floated
            if (ww != event.window.data1 && wh != event.window.data2)
            {
                CONSOLE_LOG("window event: resizing to %d, %d\n", event.window.data1, event.window.data2);
                update_window_size();
                set_tile_size(window_width, window_height);
                ww = event.window.data1;
                wh = event.window.data2;
            }
        }

        if (event.type == SDL_MOUSEBUTTONDOWN)
        {
            mouse_pressed(&event.button);
        }
        if (event.type == SDL_MOUSEWHEEL)
        {
            mouse_wheel(&event.wheel);
        }
    }
    if (!current_menu)
    {
        unsigned int now = SDL_GetTicks();
        unsigned int dt = (now - last_move);
        // printf("dt=%u now=%u last=%u\n", dt, now, last_move);
        //        if (dt > 100)
        {
            last_move = now;
            const Uint8 * currentKeyState = SDL_GetKeyboardState(NULL);
            handle_keyboard_state(currentKeyState);
        }
    }
    return ret;
}
