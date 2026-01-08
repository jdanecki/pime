#include <SDL2/SDL.h>

#include "../menu.h"
#include "../../client-common/key_handlers.h"
#include "../../client-common/key_handler.h"

extern bool show_craft;

void handle_mouse(int x, int y, int button);

KeyHandler menu_key_handlers[] = {
    { SDLK_ESCAPE, menu_handle_escape },
    { SDLK_RETURN, menu_handle_enter},
    { SDLK_DOWN, menu_go_down},
    { SDLK_UP, menu_go_up},
};

KeyHandler key_handlers[] = {
    { SDLK_ESCAPE, handle_escape },
    { SDLK_i, handle_i},
#ifndef DISABLE_NPC
    { SDLK_n, handle_n},
#endif
    { SDLK_q, quit_program}
};

bool key_pressed(int key)
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

bool handle_events()
{
    SDL_Event event;

    while (SDL_PollEvent(&event))
    {
        if (event.type == SDL_QUIT)
        {
            SDL_Quit();
            return true;
        };
        if (event.type == SDL_KEYDOWN)
        {
            int key = event.key.keysym.sym;

            if (key_pressed(key))
                return true;
        }

        if (event.type == SDL_MOUSEBUTTONDOWN)
        {
            handle_mouse(event.button.x, event.button.y, event.button.button);
        }
    }

    return 0;
}

