#include "../../dialog/dialog.h"
#include "../../dialog/d_craft.h"
#include "../../dialog/d_action.h"

#include "../../client-common/window.h"
#include "../../client-common/text.h"
#include "../../client-common/alchemist2d.h"
#include "playerUI.h"

Dialog * dialog;
DAction * actions;

extern int tile_size;
DCraft * craft;
PlayerUI * player;
bool show_craft;

int active_hotbar = 0;

bool handle_events();
NetClient * client;

void send_packet_craft(NetClient * client, uintptr_t prod_id, uintptr_t ingredients_num, const uintptr_t * iid)
{
    printf("send_packet_craft prod=%lu num=%ld id=%lu\n", prod_id, ingredients_num, *iid);
}
bool Player::set_known(Class_id cid, int el_id)
{
    printf("Player::set_known()\n");
    return true;
}

void button_left(DialogButton * button)
{
    printf("button left: id=%d\n", button->id);
    if (button->id == 1)
    {
        show_craft ^= 1;
        if (show_craft)
        {
            player->craftbar[0] = 1;
            player->craftbar[1] = 1;
        }
    }
}

void button_right(DialogButton * button)
{
    printf("button right: id=%d\n", button->id);
}

void handle_mouse(int x, int y, int button)
{
    if (show_craft)
        craft->press(x, y, button);
    else
        dialog->press(x, y, button);
    actions->press(x, y, button);
}

int main()
{
    if (init_window("test dialog", 1200, 600))
        return 1;
    if (load_font())
        return 1;

    load_textures();

    player = (PlayerUI *)calloc(sizeof(PlayerUI), 1);
    Element * el1 = new Element(new BaseElement(Form_solid, 0));
    Element * el2 = new Element(new BaseElement(Form_solid, 1));
    player->hotbar[0] = new Element2d(*el1);
    player->hotbar[1] = new Element2d(*el2);

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

        if (handle_events())
            break;

        Backend_Begin_Drawing();
        clear_window();

        dialog->draw();
        if (show_craft)
        {
            craft->update();
            craft->draw();
        }
        actions->draw();

        Backend_Update_Screen();
        Backend_End_Drawing();

        Backend_Wait();
    }
    return 0;
}
