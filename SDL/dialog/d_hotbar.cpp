#include "d_hotbar.h"
#include "../main.h"
#include "dialog.h"

extern int active_hotbar;

void change_active_hotbar(int id)
{
    active_hotbar = id;
}

DHotbar::DHotbar() : Dialog({650, 585, 500, 50}, {0, 0, 0, 0})
{
    for (int i = 0; i < 10; i++)
    {
        if (i == active_hotbar)
            this->add_box(i, {i * 50, 0, 50, 50}, {255, 255, 255, 255}, false);
        else
            this->add_box(i, {i * 50, 0, 50, 50}, {125, 125, 125, 255}, false);
        this->add_image(i, {i * 50 + 2, 2, 50 - 4, 50 - 4});
        this->add_button(i, {i * 50 + 2, 2, 50 - 4, 50 - 4}, 0, {0, 0, 0, 0}, {0, 0, 0, 0}, "", &change_active_hotbar);
    }
}

void DHotbar::update()
{
    for (int i = 0; i < 10; i++)
    {
        // IMAGES
        DialogImage * img = dynamic_cast<DialogImage *>(this->get_element_from_id(i, DialogElementType::Image));
        img->texture = NULL;
        if (player->hotbar[i])
        {
            Renderable * r = dynamic_cast<Renderable *>(player->hotbar[i]);
            img->texture = r->get_texture();
        }
        // SELECTION
        DialogBox * b = dynamic_cast<DialogBox *>(this->get_element_from_id(i, DialogElementType::Box));
        if (i == active_hotbar)
            b->color = {255, 255, 255, 255};
        else if (player->craftbar[i] == 1)
            b->color = {000, 200, 255, 255};
        else
            b->color = {125, 125, 125, 255};
    }
}
