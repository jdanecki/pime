#include "d_hotbar.h"
#include "playerUI.h"

extern int active_hotbar;
int hotbar_x=550, hotbar_y=420;

void change_active_hotbar(DialogButton * button)
{
    active_hotbar = button->id;
}

void change_craftbar(DialogButton * button)
{
    player->craftbar[button->id] ^= 1;
}

DHotbar::DHotbar() : Dialog({(Backend_Rect_Field)hotbar_x, (Backend_Rect_Field)hotbar_y, 500, 50}, {10, 10, 10, 255})
{
    for (int i = 0; i < 10; i++)
    {
        Backend_Rect_Field r1=(Backend_Rect_Field)(i*50);
        Backend_Rect_Field r2=(Backend_Rect_Field)(i*50+2);
        if (i == active_hotbar)
            add(new DialogBox(i, {r1, 0, 50, 50}, {255, 255, 255, 255}, false));
        else
            add(new DialogBox(i, {r1, 0, 50, 50}, {125, 125, 125, 255}, false));
        add(new DialogImage(i, {r2, 2, 50 - 4, 50 - 4}));
        add(new DialogButton(i, {r2, 2, 50 - 4, 50 - 4}, 0, {0, 0, 0, 0}, {0, 0, 0, 0}, "", change_active_hotbar, change_craftbar));
    }
}

void DHotbar::update()
{
    for (int i = 0; i < 10; i++)
    {
        // IMAGES
        DialogImage * img = dynamic_cast<DialogImage *>(get_element_from_id(i, DialogElementType::Image));
        img->texture_loaded=false;
        if (player->hotbar[i])
        {
            Renderable * r = dynamic_cast<Renderable *>(player->hotbar[i]);
            img->texture = r->get_texture();
            img->texture_loaded=true;
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
