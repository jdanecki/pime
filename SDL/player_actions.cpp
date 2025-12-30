#include "player_actions.h"

#include "../core/world.h"
#include "implementations/playerSDL.h"
#include "networking.h"

extern PlayerSDL * player;
extern int active_hotbar;

void put_element()
{
    InventoryElement * el = player->hotbar[active_hotbar];
    if (el)
    {
        send_packet_drop(client, el->uid);
        // player->hotbar[active_hotbar] = NULL;
        /*el->set_posittion(player.x, player.y);
        set_item_at_ppos(el, &player);
        player.inventory->remove(el);
        player.hotbar[active_hotbar]=NULL;
       CONSOLE_LOG("item %s placed\n", el->get_name());*/
    }
}

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

void server_action_tile(Server_action a, ItemLocation loc)
{
    InventoryElement * object = get_item_at(loc);
    if (object)
    {
        CONSOLE_LOG("SDL: server action %s on %s\n", server_action_name[a], object->get_name());
        send_packet_server_action_on_object(client, a, object->uid);
    }
    else
    {
        switch (a)
        {
            case SERVER_SHOW_CHUNK:
            case SERVER_TRACE_NETWORK:
                CONSOLE_LOG("SDL: server action %s\n", server_action_name[a]);
                send_packet_server_action_on_object(client, a, 0);
                break;
            default:
                CONSOLE_LOG("SDL: nothing to show\n");
                break;
        }
    }
}
