#include <stdio.h>
#include "playerUI.h"

#include "../../core/alchemist/elements.h"
#include "../../core/alchemist/random_functions.h"
#include "../../client-common/ui/window.h"
#include "../../client-common/ui/text.h"
#include "../../client-common/ui/menu/menu.h"

int tile_size = 32;
bool finish_program;
bool handle_events();
class Npc2d;
Npc2d * current_npc;

Renderable::Renderable()
{}
void Renderable::render(float el_x, float el_y, float w, float h) {}

Backend_Texture Renderable::get_texture()
{
    return Backend_Texture();
}

void send_packet_item_used_on_object(uintptr_t iid, uintptr_t oid)
{
}
void send_packet_action_on_object(int32_t a, uintptr_t oid)
{
}
void send_packet_pickup(uintptr_t id)
{
}
void send_packet_item_used_on_tile(uintptr_t iid, ItemLocation location)
{
}
void action_tile(Player_action a, InventoryElement * object)
{
}

void test_menu_handle_enter()
{
    printf("handle_enter: action=%d\n", current_menu->menu_pos->action);

    menu_handle_enter();
}
void handle_escape()
{
    show_menu();
}

void handle_i()
{
    show_menu_inventory_categories();
}

void quit_program()
{
    finish_program = true;
}

Base * get_base(uint32_t c_id, int32_t id)
{
    return nullptr;
    // return base.find(&id);
}

void handle_mouse(int x, int y, int button)
{
    printf("mouse: x=%d y=%d, button=%d\n", x, y, button);
}

int main()
{
    if (init_window("test menu", 1200, 600))
        return 1;
    if (load_font())
        return 1;
    create_game_menus();
    player = new PlayerUI(Player(0, SerializableCString("player"), ItemLocation::center(), random_range(50, 150), random_range(50, 150), random_range(50, 150)));
    for (;;)
    {
        if (handle_events())
            break;

        Backend_Begin_Drawing();
        clear_window();

        if (current_menu)
            current_menu->show();

        Backend_Update_Screen();
        Backend_End_Drawing();

        Backend_Wait();
    }
    return 0;
}
