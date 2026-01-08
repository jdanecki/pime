#include <SDL2/SDL.h>
#include <SDL2/SDL_keyboard.h>
#include <SDL2/SDL_keycode.h>
#include <SDL2/SDL_mouse.h>
#include <SDL2/SDL_scancode.h>
#include <SDL2/SDL_timer.h>

#include "../client-common/inputs.h"


int last_frame_press = 0;
Uint64 last_time = 0;

KeyHandler key_handlers[] = {
    { SDLK_F1, handle_f1 },
    { SDLK_F2, handle_f2 },
    { SDLK_F3, handle_f3 },
    { SDLK_F4, handle_f4 },
    { SDLK_F5, handle_f5 },
    { SDLK_F6, handle_f6 },
    { SDLK_F7, handle_f7 },
    { SDLK_RETURN, handle_enter },
    { SDLK_c, handle_c },
    { SDLK_1, handle_hotbar_0 },
    { SDLK_2, handle_hotbar_1 },
    { SDLK_3, handle_hotbar_2 },
    { SDLK_4, handle_hotbar_3 },
    { SDLK_5, handle_hotbar_4 },
    { SDLK_6, handle_hotbar_5 },
    { SDLK_7, handle_hotbar_6 },
    { SDLK_8, handle_hotbar_7 },
    { SDLK_9, handle_hotbar_8 },
    { SDLK_0, handle_hotbar_9 },
    { SDLK_q, put_element },
    { SDLK_BACKQUOTE, handle_prev_hotbar },
    { SDLK_TAB, handle_next_hotbar },
    { SDLK_MINUS, handle_minus },
    { SDLK_EQUALS, handle_equal },
    { SDLK_ESCAPE, handle_escape },
};


void key_pressed(int key)
{
    int num_handlers;
    KeyHandler * handlers;

    if (current_menu) {
        num_handlers = sizeof(menu_key_handlers)/sizeof(KeyHandler);
        handlers= menu_key_handlers;
    }
    else {
        num_handlers = sizeof(key_handlers)/sizeof(KeyHandler);
        handlers= key_handlers;
    }
    for (int i = 0; i < num_handlers; ++i) {
        if (key == handlers[i].key) {
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

Uint64 handle_keyboard_state(const Uint8 * keys)
{
    Uint64 current_time = SDL_GetTicks64();
    Uint64 time_period = 0;
    if (keys[SDL_SCANCODE_LSHIFT])
    {
        player->sneaking = 1;
        time_period = 200;
    }
    else
    {
        player->sneaking = 0;
        if (keys[SDL_SCANCODE_LCTRL] && player->hunger && player->thirst)
        {
            player->running = 1;
            time_period = 50;
        }
        else
        {
            player->running = 0;
            time_period = 100;
        }
    }

    if (current_menu == NULL && ((current_time - last_time > time_period) || !(last_frame_press)))
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
        print_status(0, " ");
        print_status(1, " ");
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

            key_pressed(key);
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
