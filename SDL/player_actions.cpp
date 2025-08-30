#include "player_actions.h"

#include "../core/world.h"
#include "networking.h"

extern Player * player;
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
        printf("item %s placed\n", el->get_name());*/
    }
}

void use_tile(ItemLocation loc)
{
    InventoryElement * object = get_item_at(loc);
    if (!object)
    {
        if (InventoryElement * item = player->hotbar[active_hotbar])
        {
            send_packet_item_used_on_tile(client, item->uid, loc);
            printf("plant seed\n");
        }

        return;
    }

    if (Product * item = dynamic_cast<Product *>(player->hotbar[active_hotbar]))
    {
        printf("SDL: using %s on %s\n", item->get_name(), object->get_name());
        send_packet_item_used_on_object(client, item->uid, object->uid);
        return;
    }

    send_packet_pickup(client, object->uid);
    /*

    for (int i = 0; i < CHUNK_SIZE*CHUNK_SIZE; i++)
    {
        Plant * p = world_table[map_y][map_x]->plants[i];
        if (p)
        {
            int px,py;
            p->get_posittion(&px, &py);
            if ((px == x && py == y) && ((p->type == PLANTID_watermelon) || (p->type == PLANTID_pumpkin)) && (p->grown))
            {
                Element * el;
                Element * el2;
                switch (p->type) {
                    case PLANTID_watermelon:
                        el = new Element(base_elements[ID_WATERMELON]);
                        el2 = new Element(base_elements[ID_WATERMELON_SEEDS]);
                        break;
                    case PLANTID_pumpkin:
                        el = new Element(base_elements[ID_PUMPKIN]);
                        el2 = new Element(base_elements[ID_PUMPKIN_SEEDS]);
                        break;
                }

                el->set_posittion(x, y);
                set_item_at(el, map_x, map_y, x, y);
                el2->set_posittion(x, y);
                set_item_at(el2, map_x, map_y, x, y);

                free(world_table[map_y][map_x]->plants[i]);
                world_table[map_y][map_x]->plants[i] = NULL;
            }
        }
    }*/
    /*            InventoryElement * el = player.hotbar[active_hotbar];
        if (el)
        {
            if (el->use(player.map_x, player.map_y, player.x, player.y))
                break;
            if (plant_with_seed(el, player.map_x, player.map_y, player.x, player.y))
                break;
            if ((Element *)el && (Element *)el->get_base() && ((Element *)el)->get_base()->id == ID_WATER)
            {
                if (Plant ** pp = get_plant_at_ppos(&player))
                {
                    if (Plant * p = *pp)
                    {
                        p->water += 100;
                        player.inventory->remove(el);
                        player.hotbar[active_hotbar]=NULL;
                        free(el);
                        break;
                    }
                }
            }
        }*/
}

void action_tile(Player_action a, ItemLocation loc)
{
    InventoryElement * object = get_item_at(loc);
    if (!object)
    {
        printf("SDL: nothing on tile\n");
        return;
    }

    printf("SDL: action %s on %s\n", player_action_name[a], object->get_name());
    send_packet_action_on_object(client, a, object->uid);
}

void server_action_tile(Server_action a, ItemLocation loc)
{
    InventoryElement * object = get_item_at(loc);
    if (object)
    {
        printf("SDL: server action %s on %s\n", server_action_name[a], object->get_name());
        send_packet_server_action_on_object(client, a, object->uid);
    }
    else
    {
        switch (a)
        {
            case SERVER_SHOW_CHUNK:
            case SERVER_TRACE_NETWORK:
                printf("SDL: server action %s\n", server_action_name[a]);
                send_packet_server_action_on_object(client, a, 0);
                break;
            default:
                printf("SDL: nothing to show\n");
                break;
        }
    }
}
