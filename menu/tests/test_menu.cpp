#include <stdio.h>

#include "../../core/alchemist/elements.h"

#include "../../client-common/window.h"
#include "../../client-common/text.h"

#include "../menu.h"

class NetClient
{
};

bool handle_events();
NetClient * client;

void send_packet_item_used_on_object(NetClient * client, uintptr_t iid, uintptr_t oid) {}
void send_packet_action_on_object(NetClient * client, int32_t a, uintptr_t oid) {}
void send_packet_pickup(NetClient * client, uintptr_t id) {} 
void send_packet_item_used_on_tile(NetClient * client, uintptr_t iid, ItemLocation location) {}

void hide_craft_window() {}

BasePlant * get_base_plant(size_t id)
{
    printf("get_base_plant id=%ld\n", id);
    return nullptr;
}
BaseAnimal * get_base_animal(size_t id)
{
    printf("get_base_animal id=%ld\n", id);
    return nullptr;
}

BaseElement * get_base_element(size_t id)
{
    printf("get_base_element id=%ld\n", id);
    return nullptr;
}

Base * get_base(uint32_t c_id, int32_t id)                                                                                                                                                    
{
    return nullptr;
   // return base.find(&id);
}

void handle_mouse(int x, int y, int button)
{
    printf("mouse: x=%d y=%y, button=%d\n", x, y, button);
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
