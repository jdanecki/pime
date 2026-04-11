#include "../../client-common/ui/dialog/dialog.h"
#include "../../client-common/ui/dialog/d_craft.h"
#include "../../client-common/ui/dialog/d_action.h"

#include "../../client-common/ui/window.h"
#include "../../client-common/ui/text.h"
#include "../../client-common/2d/alchemist2d.h"
#include "../../client-common/2d/texture.h"
#include "playerUI.h"

Dialog * dialog1;
DAction * actions;
DElements *d_ing, *d_prod, *d_inv;

PlayerUI * player;
bool show_craft;

int active_hotbar = 0;

bool handle_events();

void send_packet_craft(uintptr_t prod_id, uintptr_t ingredients_num, const uintptr_t * iid)
{
    printf("send_packet_craft prod=%lu %s num=%ld id=%lu\n", prod_id, prod_id < ING_COUNT ? ingredient_name[prod_id] : product_name[prod_id - ING_COUNT], ingredients_num, *iid);
}
bool Player::set_known(Class_id cid, int el_id)
{
    printf("Player::set_known()\n");
    return true;
}

void button_left(DialogButton * button)
{
    printf("button left: id=%d\n", button->id);

    switch (button->id)
    {
		case 0:
			player->inventory.show(false);
			break;
		case 1:
			show_craft ^= 1;
			if (show_craft)
			{
				player->craftbar[0] = 1;
				player->craftbar[1] = 1;
			}
			break;
		case 2:
			player->inventory.add(new Element2d(Element(new BaseElement(Form_solid, player->inventory.nr_elements))));
			d_inv->update();
			break;
    }
}

void button_right(DialogButton * button)
{
    printf("button right: id=%d\n", button->id);
    switch (button->id)
    {
    	case 2:
    		player->inventory.remove(player->inventory.tail);
    		d_inv->update();
    		break;
    }
}

void handle_mouse(int x, int y, int button)
{
    if (show_craft)
        d_craft->press(x, y, button);
    else
        dialog1->press(x, y, button);
    actions->press(x, y, button);

    d_ing->press(x, y, button);
    d_prod->press(x, y, button);
    d_inv->press(x, y, button);
}

void ing_button1(DialogButton * button)
{
	printf("ings: button1 id=%d\n", button->id);
}

void ing_button2(DialogButton * button)
{
	printf("ings: button2 id=%d\n", button->id);
}

void prod_button1(DialogButton * button)
{
	printf("prods: button1 id=%d\n", button->id);
}

void prod_button2(DialogButton * button)
{
	printf("prods: button2 id=%d\n", button->id);
}

void inv_button1(DialogButton * button)
{
	printf("inv: button1 id=%d\n", button->id);
}

void inv_button2(DialogButton * button)
{
	printf("inv: button2 id=%d\n", button->id);
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
    player->inventory.name="inventory";
    for (int i=0; i < 5; i++)
    {
    	player->inventory.add(new Element2d(Element(new BaseElement(Form_solid, i))));
    }


    dialog1 = new Dialog({50, 200, 550, 350}, {85, 125, 85, 125});
    dialog1->add(new DialogButton(0, {  1, 1, 150, 100}, 15, {125, 0, 0, 125}, {255, 255, 255, 255}, "Inventory", button_left, button_right));
    dialog1->add(new DialogButton(1, {170, 1, 150, 100}, 15, {0, 125, 0, 125}, {255, 255, 255, 255}, "Craft", button_left, button_right));
    dialog1->add(new DialogButton(2, {340, 1, 160, 100}, 15, {0, 0, 125, 125}, {255, 255, 255, 255}, "Add/Remove", button_left, button_right));

    dialog1->add(new DialogBox(0, {20, 140, 100, 100}, {0, 0, 0, 125}, true));
    dialog1->add(new DialogBox(1, {150, 140, 100, 100}, {0, 0, 0, 125}, true));

    dialog1->add(new DialogImage(0, {150, 23, 75, 75}, "textures/npc.png"));
    dialog1->add(new DialogImage(1, {0, 250, 100, 100}, "textures/player.png"));

    DialogImage *inv_img=new DialogImage(2, {0, -150, 100, 100});
    Renderable * r = dynamic_cast<Renderable *>(player->inventory.head->get_el());
    Backend_Texture texture = r->get_texture();
    inv_img->set_texture(texture);
    dialog1->add(inv_img);

    d_craft = new DCraft();
    actions = new DAction(50, 570, 10);

    d_ing  = new DElements(Backend_Rect(50, 650, 550, 200), ING_COUNT, ing_textures, ing_button1, ing_button2);
    d_prod = new DElements(Backend_Rect(50, 850, 550, 200), PROD_COUNT, prod_textures, prod_button1, prod_button2);
    d_inv  = new DElements(Backend_Rect(625, 50, 320, 200), &player->inventory, inv_button1, inv_button2);

    for (;;)
    {

        if (handle_events())
            break;

        Backend_Begin_Drawing();
        clear_window();

        dialog1->draw();
        if (show_craft)
        {
            d_craft->update();
            d_craft->draw();
        }
        actions->draw();

        d_ing->draw();
        d_prod->draw();
        d_inv->draw();

        Backend_Update_Screen();
        Backend_End_Drawing();

        Backend_Wait();
    }
    return 0;
}
