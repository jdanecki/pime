#include "player_actions.h"

#include "../core/world.h"
#include "playerUI.h"
#include "../../net/net.h"

extern PlayerUI * player;
extern int active_hotbar;

void use_tile()
{
    InventoryElement * object = get_item_at_ppos(player);

    if (object)
    {
        if (object->get_cid() == Class_Player)
        {
            CONSOLE_LOG("player=%p object=%p\n", player, object);
            if (player == object)
                goto player_object;
        }
        if (Product * item = static_cast<Product *>(player->hotbar[active_hotbar]))
        {
            CONSOLE_LOG("use_tile: using %s on %s uid=%lx\n", item->get_name(), object->get_name(), object->uid);
            send_packet_item_used_on_object(item->uid, object->uid);
            return;
        }
    }
    else
    {
    player_object:
        if (InventoryElement * item = player->hotbar[active_hotbar])
        {
            send_packet_item_used_on_tile(item->uid, player->location);
        }
    }
}

void pickup_item()
{
    InventoryElement * object = get_item_at_ppos(player);
    if (object)
    {
        send_packet_pickup(object->uid);
    }
}
