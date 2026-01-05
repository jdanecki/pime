#include "player_actions.h"

#include "../core/world.h"
#include "playerUI.h"
#include "../client-common/net.h"

extern PlayerUI * player;
extern int active_hotbar;

void use_tile()
{
    InventoryElement * object = get_item_at(player->location);

    if (object)
    {
        if (object->get_cid() == Class_Player)
        {
            CONSOLE_LOG("player=%p object=%p\n", player, object);
            if (player == object) goto player_object;
        }
        if (Product * item = dynamic_cast<Product *>(player->hotbar[active_hotbar]))
        {
            CONSOLE_LOG("SDL: using %s on %s uid=%lx\n", item->get_name(), object->get_name(), object->uid);
            send_packet_item_used_on_object(client, item->uid, object->uid);
            return;
        }

        send_packet_pickup(client, object->uid);
    }
    else
    {
player_object:
        if (InventoryElement * item = player->hotbar[active_hotbar])
        {
            send_packet_item_used_on_tile(client, item->uid, player->location);
        }
    }
}

void action_tile(Player_action a, ItemLocation loc)
{
    InventoryElement * object = get_item_at(loc);
    if (!object)
    {
        CONSOLE_LOG("SDL: nothing on tile\n");
        return;
    }

    CONSOLE_LOG("SDL: action %s on %s\n", player_action_name[a], object->get_name());
    send_packet_action_on_object(client, a, object->uid);
}


