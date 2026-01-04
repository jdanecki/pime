#include "dialog.h"
#include "../window.h"
#include "../text.h"
#include "d_craft.h"
#include "d_action.h"
#include "../main.h"
#include "../implementations/alchemistSDL.h"

Dialog * dialog;
DAction * actions;

extern int tile_size;
DCraft *craft;
PlayerSDL * player;
bool show_craft;

int active_hotbar = 0;

NetClient * client;

BasePlant * get_base_plant(size_t id)
{
    printf("get_base_plant id=%d\n", id);
    return nullptr;
}
BaseAnimal * get_base_animal(size_t id)
{
    printf("get_base_animal id=%d\n", id);
    return nullptr;
}

BaseElement * get_base_element(size_t id)
{
    printf("get_base_element id=%d\n", id);
    return nullptr;
}

void send_packet_craft(NetClient * client, uintptr_t prod_id, uintptr_t ingredients_num, const uintptr_t * iid)
{
    printf("send_packet_craft prod=%lu num=%ld id=%lu\n", prod_id, ingredients_num, *iid);
}
bool Player::set_known(Class_id cid, int el_id)
{
    printf("Player::set_known()\n");
    return true;
}

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
    if (show_craft)
        craft->press(event.x, event.y, event.button);
    else
        dialog->press(event.x, event.y, event.button);
    actions->press(event.x, event.y, event.button);
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

void button_left(DialogButton * button)
{
    printf("button left: id=%d\n", button->id);
    if (button->id==1){
        show_craft^=1;
        if (show_craft)
        {
            player->craftbar[0]=1;
            player->craftbar[1]=1;
        }
    }
}

void button_right(DialogButton * button)
{
    printf("button right: id=%d\n", button->id);
}

int main()
{
    if (init_window("test dialog", 1200, 600))
        return 1;
    if (load_font())
        return 1;

    load_textures();

    player = (PlayerSDL *)calloc(sizeof(PlayerSDL), 1);
    Element * el1 = new Element(new BaseElement(Form_solid, 0));
    Element * el2 = new Element(new BaseElement(Form_solid, 1));
    player->hotbar[0]=new ElementSDL(*el1);
    player->hotbar[1]=new ElementSDL(*el2);

    dialog = new Dialog({50, 200, 550, 350}, {125, 125, 125, 125});
    dialog->add(new DialogButton(0, {0, 0, 150, 100}, 15, {125, 0, 0, 125}, // bg
        {255, 255, 255, 255},                                               // fg
        "Button 0", button_left, button_right));
    dialog->add(new DialogButton(1, {170, 0, 150, 100}, 15, {0, 125, 0, 125}, {255, 255, 255, 255}, "Button 1", button_left, button_right));
    dialog->add(new DialogButton(2, {340, 0, 150, 100}, 15, {0, 0, 125, 125}, {255, 255, 255, 255}, "Button 2", button_left, button_right));

    dialog->add(new DialogBox(0, {20, 140, 100, 100}, {0, 0, 0, 125}, true));
    dialog->add(new DialogBox(1, {150, 140, 100, 100}, {0, 0, 0, 125}, true));

    dialog->add(new DialogImage(0, {150, 23, 75, 75}, "textures/npc.png"));
    dialog->add(new DialogImage(1, {0, 250, 100, 100}, "textures/player.png"));

    craft = new DCraft();

    actions = new DAction(10);

    for (;;)
    {
        clear_window();

        if (handle_SDL_events())
            break;

        dialog->draw(renderer);
        if (show_craft) {
            craft->update();
            craft->draw(renderer);
        }        
        actions->draw(renderer);

        SDL_RenderPresent(renderer);
        SDL_Delay(20);
    }
    return 0;
}
