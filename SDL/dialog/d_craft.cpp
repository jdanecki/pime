#include "d_craft.h"
#include "../main.h"
#include "dialog.h"
#include <cstdio>

extern int active_hotbar;
extern NetClient * client;

DCraft d_craft;

bool craft2elements(Product_id what)
{
    InventoryElement *el1 = nullptr, *el2 = nullptr;
    int i1, i2;
    for (int i = 0; i < 10; i++)
    {
        if (player->craftbar[i])
        {
            if (!el1)
            {
                el1 = player->hotbar[i];
                i1 = i;
            }
            else
            {
                el2 = player->hotbar[i];
                i2 = i;
                break;
            }
        }
    }
    if (el1 && el2)
    {
        size_t ingredients[2] = {el1->uid, el2->uid};
        send_packet_craft(client, ING_COUNT + what, 2, ingredients);
        // FIXME what if crafting will fail?
        player->set_known(Class_Product, what);

        player->craftbar[i1] = 0;
        player->craftbar[i2] = 0;
        return true;
    }
    return false;
}

void button_craft_ing(int id)
{
    printf("Crafting %d\n", id);
    if (player->hotbar[active_hotbar])
    {
        send_packet_craft(client, id, 1, &player->hotbar[active_hotbar]->uid);
        // FIXME what if crafting will fail?
        player->set_known(Class_Ingredient, id);
    }
}

void button_craft_prod(int id)
{
    craft2elements((Product_id)id);
}

void button_switch(int id)
{
    if (id == 0)
    {
        d_craft.in_products = 0;
    }
    if (id == 1)
    {
        d_craft.in_products = 1;
    }
}

DCraft::DCraft() : Dialog({50, 50, 500, 500}, {125, 125, 125, 125}), ingredients({190, 170, 500 - 140, 500 - 120}, {125, 125, 125, 10}), products({190, 170, 500 - 140, 500 - 120}, {125, 125, 125, 10})
{
    this->show = false;
    this->add_button(0, {0, 0, 250, 100}, 15, {0, 0, 0, 125}, {255, 255, 255, 255}, "Ingredients", &button_switch);
    this->add_button(1, {250, 0, 250, 100}, 15, {0, 0, 0, 125}, {255, 255, 255, 255}, "Products", &button_switch);
    this->add_box(0, {20, 120, 100, 100}, {0, 0, 0, 125}, true);
    this->add_box(1, {20, 240, 100, 100}, {0, 0, 0, 125}, true);
    this->add_image(0, {20, 120, 100, 100});
    this->add_image(1, {20, 240, 100, 100});

    for (int i = 0; i < ING_COUNT; i++)
    {
        int x = i % 6 * 54;
        int y = i / 6 * 54;
        this->ingredients.add_button(i, {x, y, 50, 50}, 10, {0, 0, 0, 125}, {0, 0, 0, 0}, "", &button_craft_ing);
        this->ingredients.add_image(i, {x + 2, y + 2, 46, 46});
    }

    for (int i = 0; i < PROD_ELEMENTS; i++)
    {
        int x = i % 6 * 54;
        int y = i / 6 * 54;
        this->products.add_button(i, {x, y, 50, 50}, 10, {0, 0, 0, 125}, {0, 0, 0, 0}, "", &button_craft_prod);
        this->products.add_image(i, {x + 2, y + 2, 46, 46});
    }
}

void DCraft::draw(SDL_Renderer * renderer)
{
    Dialog::draw(renderer);
    if (this->in_products)
        this->products.draw(renderer);
    else
        this->ingredients.draw(renderer);
}

void DCraft::press(int x, int y)
{
    Dialog::press(x, y);
    if (this->in_products)
        this->products.press(x, y);
    else
        this->ingredients.press(x, y);
}

void DCraft::update()
{
    if (this->in_products)
    {
        InventoryElement *el1 = nullptr, *el2 = nullptr;
        for (int i = 0; i < 10; i++)
        {
            if (player->craftbar[i])
            {
                if (!el1)
                {
                    el1 = player->hotbar[i];
                }
                else
                {
                    el2 = player->hotbar[i];
                    break;
                }
            }
        }
        DialogImage * img = dynamic_cast<DialogImage *>(this->get_element_from_id(0, DialogElementType::Image));
        img->texture = NULL;
        if (el1)
        {
            Renderable * r = dynamic_cast<Renderable *>(el1);
            img->texture = r->get_texture();
        }
        img = dynamic_cast<DialogImage *>(this->get_element_from_id(1, DialogElementType::Image));
        img->texture = NULL;
        if (el2)
        {
            Renderable * r = dynamic_cast<Renderable *>(el2);
            img->texture = r->get_texture();
        }
    }
    else
    {
        DialogImage * img = dynamic_cast<DialogImage *>(this->get_element_from_id(0, DialogElementType::Image));
        img->texture = NULL;
        if (player->hotbar[active_hotbar])
        {
            Renderable * r = dynamic_cast<Renderable *>(player->hotbar[active_hotbar]);
            img->texture = r->get_texture();
        }
    }

    DialogButton * ing_button = dynamic_cast<DialogButton *>(this->get_element_from_id(0, DialogElementType::Button));
    DialogButton * prod_button = dynamic_cast<DialogButton *>(this->get_element_from_id(1, DialogElementType::Button));
    DialogBox * box = dynamic_cast<DialogBox *>(this->get_element_from_id(1, DialogElementType::Box));
    if (in_products)
    {
        ing_button->d_box->color = {125, 125, 125, 5};
        prod_button->d_box->color = {125, 125, 125, 125};
        box->color.a = 125;
    }
    else
    {
        ing_button->d_box->color = {125, 125, 125, 125};
        prod_button->d_box->color = {125, 125, 125, 5};
        box->color.a = 0;
    }

    DialogImage * img = dynamic_cast<DialogImage *>(this->ingredients.get_element_from_id(0, DialogElementType::Image));
    if (img->texture)
        return;
    for (int i = 0; i < ING_COUNT; i++)
    {
        DialogImage * img = dynamic_cast<DialogImage *>(this->ingredients.get_element_from_id(i, DialogElementType::Image));
        img->texture = ing_textures[i];
    }

    for (int i = 0; i < PROD_ELEMENTS; i++)
    {
        DialogImage * img = dynamic_cast<DialogImage *>(this->products.get_element_from_id(i, DialogElementType::Image));
        img->texture = prod_textures[i];
    }
}
