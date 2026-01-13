#include <stdio.h>

#include "../../core/alchemist/elements.h"

#include "../../client-common/window.h"
#include "../../client-common/text.h"

#include "../../menu/menu.h"

bool finish_program;
bool handle_events();
class NetClient
{
};

NetClient * client;

void send_packet_item_used_on_object(NetClient * client, uintptr_t iid, uintptr_t oid)
{
}
void send_packet_action_on_object(NetClient * client, int32_t a, uintptr_t oid)
{
}
void send_packet_pickup(NetClient * client, uintptr_t id)
{
}
void send_packet_item_used_on_tile(NetClient * client, uintptr_t iid, ItemLocation location)
{
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
    create_menus();
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
