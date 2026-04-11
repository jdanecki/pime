#include <stdio.h>
#include "d_craft.h"
#include "dialog.h"
#include "../../../net/net.h"
#include "playerUI.h"

extern int active_hotbar;
extern NetClient * client;

DCraft *d_craft;

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
        send_packet_craft(ING_COUNT + what, 2, ingredients);
        // FIXME what if crafting fail?
        player->set_known(Class_Product, what);

        player->craftbar[i1] = 0;
        player->craftbar[i2] = 0;
        return true;
    }
    return false;
}

void button_craft_ing(DialogButton * button)
{
    CONSOLE_LOG("Crafting ing: %d %s\n", button->id, ingredient_name[button->id]);
    if (player->hotbar[active_hotbar])
    {
        send_packet_craft(button->id, 1, &player->hotbar[active_hotbar]->uid);
        // FIXME what if crafting fail?
        player->set_known(Class_Ingredient, button->id);
    }
}
void show_craft_ing(DialogButton * button)
{
    auto text = button->dialog->get<DialogText>(0);
    if (text)
        text->change_text(ingredient_name[button->id]);
}

void button_craft_prod(DialogButton * button)
{
    CONSOLE_LOG("Crafting prod: %d %s\n", button->id, product_name[button->id]);
    craft2elements((Product_id)button->id);
}

void show_prod_ing(DialogButton * button)
{
	auto text = button->dialog->get<DialogText>(0);
    if (text)
        text->change_text(product_name[button->id]);
}

void button_switch(DialogButton * button)
{
    printf("button_switch: id=%d\n", button->id);
    DCraft * cr = static_cast<DCraft *>(button->dialog);
    if (cr)
        cr->in_products = button->id;
}

DElements::DElements(Backend_Rect r, ElementsList *list, void (*on_press)(DialogButton *), void (*on_secondary_press)(DialogButton *)):
		Dialog(r, {125, 125, 125, 10})
{
	el_list=list;
	button1_press=on_press;
	button2_press=on_secondary_press;
	add_elems();
}

void DElements::add_elems()
{
	int i=0;
	for (InventoryElement * el : *el_list)
	{
		int x = i % 6 * 54;
		int y = i / 6 * 54;

		Renderable * r = dynamic_cast<Renderable *>(el);
		Backend_Texture texture = r->get_texture();
		add(new DialogButton(i, Backend_Rect(x, y, 50, 50), 10, {0, 0, 0, 125}, {0, 0, 0, 0}, texture, button1_press, button2_press));
		i++;
	}
}
void DElements::update()
{
	printf("DElements::update\n");
	clear_elements();
	add_elems();
}

DElements::DElements(Backend_Rect r, int count, Backend_Texture *textures, void (*on_press)(DialogButton *), void (*on_secondary_press)(DialogButton *)):
		Dialog(r, {125, 125, 125, 10})
{
	button1_press=on_press;
	button2_press=on_secondary_press;

	for (int i = 0; i < count; i++)
	{
		int x = i % 6 * 54;
		int y = i / 6 * 54;
		add(new DialogButton(i, Backend_Rect(x, y, 50, 50), 10, {0, 0, 0, 125}, {0, 0, 0, 0}, textures[i], button1_press, button2_press));
	}
	el_list=nullptr;
}
DCraft::DCraft()
    : Dialog(Backend_Rect(60, 60, 500, 500), {125, 125, 125, 255}),
	  ingredients(Backend_Rect(190, 170, 500 - 140, 500 - 120), {125, 125, 125, 10}),
      products(Backend_Rect(190, 170, 500 - 140, 500 - 120), {125, 125, 125, 10})
{
    show = false;
    in_products = false;
    add(new DialogButton(0, Backend_Rect(1, 1, 250, 100), 15, {0, 0, 0, 125}, {255, 255, 255, 255}, "Ingredients", button_switch));
    add(new DialogButton(1, Backend_Rect(250, 1, 250, 100), 15, {0, 0, 0, 125}, {255, 255, 255, 255}, "Products", button_switch));
    add(new DialogBox(0, Backend_Rect(20, 120, 100, 100), {0, 0, 0, 125}, true));
    add(new DialogBox(1, Backend_Rect(20, 240, 100, 100), {0, 0, 0, 125}, true));
    add(new DialogImage(0, Backend_Rect(21, 121, 98, 98)));
    add(new DialogImage(1, Backend_Rect(21, 241, 98, 98)));
    for (int i = 0; i < ING_COUNT; i++)
    {
        int x = i % 6 * 54;
        int y = i / 6 * 54;
        ingredients.add(new DialogButton(i, Backend_Rect(x, y, 50, 50), 10, {0, 0, 0, 125}, {0, 0, 0, 0}, ing_textures[i], button_craft_ing, show_craft_ing));
    }
    ingredients.add(new DialogText(0, 0, (1 + ING_COUNT / 6) * 54, 20, {0, 0, 0, 255}, "Ingredient element"));

    for (int i = 0; i < PROD_COUNT; i++)
    {
        int x = i % 6 * 54;
        int y = i / 6 * 54;
        products.add(new DialogButton(i, Backend_Rect(x, y, 50, 50), 10, {0, 0, 0, 125}, {0, 0, 0, 0}, prod_textures[i], button_craft_prod, show_prod_ing));


    }
    products.add(new DialogText(0, 0, (1 + PROD_COUNT / 6) * 54, 20, {0, 0, 0, 255}, "Product element"));
}

void DCraft::draw()
{
    Dialog::draw();
    if (in_products)
        products.draw();
    else
        ingredients.draw();
}

bool DCraft::press(int x, int y, int button)
{
    Dialog::press(x, y, button);
    if (in_products)
    {
        auto text = products.get<DialogText>(0);
        if (text)
            text->change_text("Product element");
        return products.press(x, y, button);
    }
    else
    {
    	auto text = ingredients.get<DialogText>(0);
        if (text)
            text->change_text("Ingredient element");
        return ingredients.press(x, y, button);
    }
}

void DCraft::update()
{
	auto ing_button = get<DialogButton>(0);
	auto prod_button = get<DialogButton>(1);
	auto box = get<DialogBox>(1);
	auto img0 = get<DialogImage>(0);
	auto img1 = get<DialogImage>(1);
	img0->visible = false;
	img1->visible = false;

    if (in_products)
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
        if (el1)
        {
            Renderable * r = dynamic_cast<Renderable *>(el1);
            img0->set_texture(r->get_texture());
        }
        if (el2)
        {
            Renderable * r = dynamic_cast<Renderable *>(el2);
            img1->set_texture(r->get_texture());
        }
        ing_button->d_box->color = {125, 125, 125, 5};
  	    prod_button->d_box->color = {125, 255, 125, 125};
 	    box->visible = true;
    }
    else
    {
        if (player->hotbar[active_hotbar])
        {
            Renderable * r = dynamic_cast<Renderable *>(player->hotbar[active_hotbar]);
            img0->set_texture(r->get_texture());
        }

        ing_button->d_box->color = {125, 255, 125, 125};
	    prod_button->d_box->color = {125, 125, 125, 5};
	    box->visible = false;
    }
}

bool hide_craft_window()
{
    if (d_craft->show)
    {
        d_craft->show = false;
        return true;
    }
    return false;
}

void create_dialogs()
{
	d_craft = new DCraft();
}
