#include <SDL2/SDL.h>

extern bool show_craft;

void handle_mouse(int x, int y, int button);

bool key_pressed(int key)
{
    switch (key)
    {
        case SDLK_ESCAPE:
            if (show_craft) {
                show_craft=0;
            }
            else
                return true;
    }
    return false;
}

void mouse_pressed(SDL_MouseButtonEvent & event)
{
    printf("mouse x: %d y: %d button=%d\n", event.x, event.y, event.button);
    handle_mouse(event.x, event.y, event.button);
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
            mouse_pressed(event.button);
        }
    }

    return 0;
}

