#include "dialog.h"
#include "../window.h"
#include "../text.h"

Dialog * dialog;

bool key_pressed(int key)
{
    switch (key)
    {
        case SDLK_ESCAPE:
            return true;
    }
    return false;
}

void mouse_pressed(SDL_MouseButtonEvent & event)
{
    printf("mouse x: %d y: %d button=%d\n", event.x, event.y, event.button);
    dialog->press(event.x, event.y, event.button);
}

bool handle_SDL_events()
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

void draw()
{
    dialog->draw(renderer);
}

void button_left(DialogButton * button)
{
    printf("button left: id=%d\n", button->id);
}

void button_right(DialogButton * button)
{
    printf("button right: id=%d\n", button->id);
}

int main()
{
    if (init_window())
        return 1;
    if (load_font())
        return 1;

    dialog = new Dialog({50, 50, 750, 500}, {125, 125, 125, 125});
    dialog->add(new DialogButton(0, {0, 0, 250, 100}, 15, {125, 0, 0, 125}, // bg
        {255, 255, 255, 255},                                               // fg
        "Button 0", button_left, button_right));
    dialog->add(new DialogButton(1, {250, 0, 250, 100}, 15, {0, 125, 0, 125}, {255, 255, 255, 255}, "Button 1", button_left, button_right));
    dialog->add(new DialogButton(2, {500, 0, 250, 100}, 15, {0, 0, 125, 125}, {255, 255, 255, 255}, "Button 2", button_left, button_right));

    dialog->add(new DialogBox(0, {20, 120, 100, 100}, {0, 0, 0, 125}, true));
    dialog->add(new DialogBox(1, {20, 240, 100, 100}, {0, 0, 0, 125}, true));

    dialog->add(new DialogImage(0, {20, 360, 100, 100}, "textures/npc.png"));
    dialog->add(new DialogImage(1, {140, 360, 100, 100}, "textures/player.png"));

    for (;;)
    {
        clear_window();

        if (handle_SDL_events())
            break;

        draw();

        SDL_RenderPresent(renderer);
        SDL_Delay(20);
    }
    return 0;
}
