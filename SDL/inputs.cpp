#include <SDL2/SDL.h>
#include <SDL2/SDL_keyboard.h>
#include <SDL2/SDL_keycode.h>
#include <SDL2/SDL_mouse.h>
#include <SDL2/SDL_scancode.h>
#include <SDL2/SDL_timer.h>

#include "../client-common/inputs.h"

KeyHandler menu_key_handlers[] = {
    {SDLK_ESCAPE, menu_handle_escape, true},
    {SDLK_RETURN, menu_handle_enter, true},
    {SDLK_DOWN, menu_go_down, false},
    {SDLK_UP, menu_go_up, false},
};

KeyHandler key_handlers[] = {
    {SDLK_F1, handle_f1, true},
    {SDLK_F2, handle_f2, true},
    {SDLK_F3, handle_f3, true},
    {SDLK_F4, handle_f4, true},
    {SDLK_F5, handle_f5, true},
    {SDLK_F6, handle_f6, true},
    {SDLK_F7, handle_f7, true},
    {SDLK_RETURN, handle_enter, true},
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
    {SDLK_c, handle_c, true},
    {SDLK_i, handle_i, true},
#ifndef DISABLE_NPC
    {SDLK_n, handle_n, true},
#endif
    {SDLK_q, put_element, true},

    {SDLK_BACKQUOTE, handle_prev_hotbar, false},
    {SDLK_TAB, handle_next_hotbar, false},
    {SDLK_MINUS, handle_minus, true},
    {SDLK_EQUALS, handle_equal, true},
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

int last_frame_press = 0;
Uint64 last_time = 0;
Uint64 handle_keyboard_state(const Uint8 * keys)
{
    Uint64 current_time = SDL_GetTicks64();
    Uint64 time_period = 0;
    if (keys[SDL_SCANCODE_LSHIFT])
    {
        handle_left_shift();
        time_period = 200;
    }
    else
    {
        player->sneaking = 0;
        if (keys[SDL_SCANCODE_LCTRL] && player->hunger && player->thirst)
        {
            handle_left_control();
            time_period = 50;
        }
        else
        {
            player->running = 0;
            time_period = 100;
        }
    }

    if ((!current_menu) && ((current_time - last_time > time_period) || !(last_frame_press)))
    {
        if (keys[SDL_SCANCODE_DOWN])
        {
            send_packet_move(client, 0, 1);
            last_frame_press = 1;
        }
        else if (keys[SDL_SCANCODE_UP])
        {
            send_packet_move(client, 0, -1);
            last_frame_press = 1;
        }
        if (keys[SDL_SCANCODE_RIGHT])
        {
            player->going_right = 1;
            send_packet_move(client, 1, 0);
            last_frame_press = 1;
        }
        else if (keys[SDL_SCANCODE_LEFT])
        {
            player->going_right = 0;
            send_packet_move(client, -1, 0);
            last_frame_press = 1;
        }
        if (last_frame_press)
        {
            return SDL_GetTicks64();
        }
    }
    if (keys[SDL_SCANCODE_DOWN] || keys[SDL_SCANCODE_UP] || keys[SDL_SCANCODE_RIGHT] || keys[SDL_SCANCODE_LEFT])
    {
        last_frame_press = 1;
        return 0;
    }

    last_frame_press = 0;
    return 0;
}

bool handle_events()
{
    int ww = 0, wh = 0;
    SDL_Event event;

    // keyboard handling for not move
    while (SDL_PollEvent(&event))
    {
        if (event.type == SDL_QUIT)
        {
            return true;
        };
        if (event.type == SDL_KEYDOWN)
        {
            int key = event.key.keysym.sym;

            return key_pressed(key);
        }
        if (event.type == SDL_WINDOWEVENT && event.window.event == SDL_WINDOWEVENT_RESIZED)
        {
            // i3 window manager sends these events if window is not floated
            if (ww != event.window.data1 && wh != event.window.data2)
            {
                CONSOLE_LOG("window event: resizing to %d, %d\n", event.window.data1, event.window.data2);
                update_window_size();
                ww = event.window.data1;
                wh = event.window.data2;
            }
        }

        if (event.type == SDL_MOUSEBUTTONDOWN)
        {
            mouse_pressed(&event.button);
        }
    }
    const Uint8 * currentKeyState = SDL_GetKeyboardState(NULL);
    Uint64 tmp = handle_keyboard_state(currentKeyState);
    if (tmp > 0)
    {
        last_time = tmp;
    }
    return false;
}
