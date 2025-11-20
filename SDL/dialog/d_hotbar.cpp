#include "d_hotbar.h"
#include "../main.h"
#include "dialog.h"

extern int active_hotbar;

void change_active_hotbar(DialogButton * button)
{
    active_hotbar = button->id;
}

void change_craftbar(DialogButton * button)
{
    player->craftbar[button->id] ^= 1;
}

DHotbar::DHotbar() : Dialog({550, 490, 500, 50}, {0, 0, 0, 0})
{
    for (int i = 0; i < 10; i++)
    {
        if (i == active_hotbar)
            add(new DialogBox(i, {i * 50, 0, 50, 50}, {255, 255, 255, 255}, false));
        else
            add(new DialogBox(i, {i * 50, 0, 50, 50}, {125, 125, 125, 255}, false));
        add(new DialogImage(i, {i * 50 + 2, 2, 50 - 4, 50 - 4}));
        add(new DialogButton(i, {i * 50 + 2, 2, 50 - 4, 50 - 4}, 0, {0, 0, 0, 0}, {0, 0, 0, 0}, "", change_active_hotbar, change_craftbar));
    }
}

void DHotbar::update()
{
    for (int i = 0; i < 10; i++)
    {
        // IMAGES
        DialogImage * img = dynamic_cast<DialogImage *>(get_element_from_id(i, DialogElementType::Image));
        img->texture = NULL;
        if (player->hotbar[i])
        {
            Renderable * r = dynamic_cast<Renderable *>(player->hotbar[i]);
            img->texture = r->get_texture();
        }
        // SELECTION
        DialogBox * b = dynamic_cast<DialogBox *>(get_element_from_id(i, DialogElementType::Box));
        if (i == active_hotbar)
        {
            b->color = {150, 150, 80, 255};
            b->fill = true;
        }
        else {
            b->fill = false;
            b->color = {125, 125, 125, 255};
        }
        if (player->craftbar[i] == 1) {
            if (b->fill)
                b->color = {000, 240, 40, 255};
            else
            {
                b->fill=true;
                b->color = {000, 150, 40, 255};
            }
        }
    }
}
