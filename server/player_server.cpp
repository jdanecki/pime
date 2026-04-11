#include <stdio.h>

#include "player_server.h"
#include "elements/being_server.h"
#include "elements_server.h"
#include "../core/alchemist/random_functions.h"
#include "networking.h"
#include "../core/packet_types.h"
#include "places/places.h"

ElementsList * players;

void create_players()
{
    players = new ElementsList("Players");
}

void PlayerServer::move_by(float dx, float dy)
{
    ItemLocation old = location;
    BeingServer::move_by(this, dx, dy);

    update_location(NetworkObject(get_cid(), get_uid()), old, location);
    // CONSOLE_LOG("SERV: player moved [%d,%d][%d,%d]\n", new_map_x, new_map_y, new_x, new_y);
}

bool PlayerServer::use_item_on_object(InventoryElement * item, InventoryElement * object)
{
    ProductServer * prod = dynamic_cast<ProductServer *>(item);
    if (prod)
    {
        CONSOLE_LOG("%s: using product: %s on object: %s\n", get_name(), prod->get_name(), object->get_name());
        if (prod->use_on(object, this))
        {
            return true;
        }
    }
    else
    	switch(object->get_cid())
    	{
			case Class_Product:
			{
				ProductServer * obj_prod = dynamic_cast<ProductServer *>(object);
				if (obj_prod)
				{
					CONSOLE_LOG("%s: using item: %s on product: %s\n", get_name(), item->get_name(), obj_prod->get_name());
					if (obj_prod->use_on(item, this))
					{
						return true;
					}
				}
				break;
			}

			case Class_Barn:
			{
				BarnServer * obj_barn = dynamic_cast<BarnServer *>(object);
				CONSOLE_LOG("%s: using item: %s on %s\n", get_name(), item->get_name(), obj_barn->get_name());
				if (obj_barn->use_on(item, this))
				{
					return true;
				}
				break;
			}
    	}
    return false;
}

bool PlayerServer::action_on_object(Player_action a, InventoryElement * object)
{
    if (!object)
        return false;
    CONSOLE_LOG("%s action: %s on %s\n", get_name(), player_action_name[a], object->get_name());
    switch (a)
    {
        case PLAYER_CHECK:
            CONSOLE_LOG("checking %s:\n", object->get_name());
            if (set_checked(object->uid))
            {
                notify_checked(get_id(), object->uid);
            }
            else
            {
                CONSOLE_LOG("%s: already checked this item\n", object->get_name());
            }
            break;
        case PLAYER_EAT:
        case PLAYER_DRINK:
            if (object->player_action(a, this))
            {
                notify_update(this);
            }
            break;
        default:
            object->player_action(a, this);
            break;
    }
    return true;
}

bool PlayerServer::server_action_on_object(Server_action a, InventoryElement * object)
{
    if (object)
        CONSOLE_LOG("%s server action: %s on %s\n", get_name(), server_action_name[a], object->get_name());
    else
        CONSOLE_LOG("%s server action: %s\n", get_name(), server_action_name[a]);
    switch (a)
    {
        case SERVER_SHOW_ITEM:
            if (object)
                object->show(true);
            break;
        case SERVER_SHOW_CHUNK:
            show_chunk(location);
            break;
    }

    return true;
}

bool PlayerServer::use_product_on_tile(Product * prod, int map_x, int map_y, int x, int y)
{
    ProductServer * prod_serv = static_cast<ProductServer *>(prod);
    if (!prod_serv)
    {
        return false;
    }
    CONSOLE_LOG("%s: using %s on tile (%d,%d):(%d,%d)\n", get_name(), prod_serv->get_name(), map_x, map_y, x, y);
    return prod_serv->use_tile(map_x, map_y, x, y, this);
}

bool PlayerServer::plant_with_seed(InventoryElement * el, int map_x, int map_y, int x, int y)
{

#if 0
    if (get_tile_at_ppos(this) == TILE_GRASS || get_tile_at_ppos(this) == TILE_DIRT)
    {
        int id = el->get_id();
        if (id == ID_ACORN || id == ID_ACORN1 || id == ID_ACORN2)
        {
            Plant * p = new PlantServer();
            switch (id)
            {
                case ID_ACORN:
                    p->type = PLANTID_tree;
                    break;
                case ID_ACORN1:
                    p->type = PLANTID_tree1;
                    break;
                case ID_ACORN2:
                    p->type = PLANTID_tree2;
                    break;
                case ID_PUMPKIN_SEEDS:
                    p->type = PLANTID_pumpkin;
                    break;
                case ID_WATERMELON_SEEDS:
                    p->type = PLANTID_watermelon;
                    break;
                case ID_STRAWBERRY_SEEDS:
                    p->type = PLANTID_strawberry;
                    break;
                default:
                    delete p;
                    p = nullptr;
            }
            if (!p)
                return false;
            ListElement * le = world_table[map_y][map_x]->objects.head;
            while (le)
            {
                if (le->el->location.data.chunk.x == x && le->el->location.data.chunk.y == y)
                    return false;
                le = le->next;
            }
            world_table[map_y][map_x]->add_object(p, x, y);
            objects_to_create.add(p);

            p->phase = Plant_seedling;
            p->grown = false;
            p->age->value = 1;

            drop(el);
            destroy(el);
            return true;
        }
    }
#endif
    return false;
}

bool PlayerServer::pickup(InventoryElement * item)
{
    if (!item->can_pickup())
    {
        CONSOLE_LOG("can't pickup %s\n", item->get_name());
        return false;
    }
    ItemLocation old_location = item->location;
    remove_from_chunks(item);
    Player::pickup(item);
    update_location(NetworkObject(item->get_cid(), item->get_uid()), old_location, item->location);
    return true;
}

bool PlayerServer::tick()
{
    if (hunger_delay-- <= 0)
    {
        hunger -= 1;
        thirst -= 1;
        hunger_delay = hunger_delay_max;
        notify_update(this);
    }
    return true;
}

PlayerServer::PlayerServer(size_t uid)
    : Player(uid, SerializableCString("player"), ItemLocation::center(), random_range(50, 150), random_range(50, 150), random_range(50, 150)), hunger_delay(600), hunger_delay_max(600)
{
    CONSOLE_LOG("PlayerServer: uid=%ld\n", uid);
    notify_create(this);
}

void PlayerServer::show(bool details)
{

    Player::show(details);
    BeingServer::show(details);
}

PlayerServer * create_player(size_t id)
{
    CONSOLE_LOG("CREATE PLAYER SERVER\n\n");
    return new PlayerServer(id);
}
void show_players()
{
    ListElement * pl_el = players->head;
    while (pl_el)
    {
        PlayerClient * pl = static_cast<PlayerClient *>(pl_el);
        pl->show();
        pl_el = pl_el->next;
    }
}
