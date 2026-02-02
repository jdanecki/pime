#include <stdio.h>

#include "player_server.h"
#include "elements/being_server.h"
#include "elements_server.h"
#include "../core/alchemist/random_functions.h"
#include "networking.h"
#include "world_server.h"
#include "../core/packet_types.h"

ElementsList * players;
ElementsList * npcs;
int npcs_id = 100;

void create_players()
{
    players = new ElementsList("Players");
    npcs = new ElementsList("NPCs");
    NPCServer * npc1 = new NPCServer(npcs_id++);
    npcs->add(npc1);
    add_object_to_world(npc1, npc1->location);

    NPCServer * npc2 = new NPCServer(npcs_id++);
    npcs->add(npc2);
    add_object_to_world(npc2, npc2->location);
}

bool check_and_load_chunk(int new_map_x, int new_map_y)
{
    int size = 5;
    for (int cy = new_map_y - size; cy <= new_map_y + size; cy++)
    {
        if (cy >= 0 && cy < WORLD_SIZE)
        {
            for (int cx = new_map_x - size; cx <= new_map_x + size; cx++)
            {
                if (cx >= 0 && cx < WORLD_SIZE)
                {
                    if (!world_table[cy][cx])
                        load_chunk(cx, cy);
                }
                else
                    return false;
            }
        }
        else
            return false;
    }
    return true;
}

void PlayerServer::move(float dx, float dy)
{
    // CONSOLE_LOG("SERV: player move dx=%f dy=%f\n", dx, dy);
    ItemLocation old = location;

    float new_x = location.chunk.x + dx;
    float new_y = location.chunk.y + dy;
    int new_map_x = location.chunk.map_x;
    int new_map_y = location.chunk.map_y;

    if (new_x < 0)
    {
        new_map_x = location.chunk.map_x - 1;
        new_x += CHUNK_SIZE;
    }
    if (new_y < 0)
    {
        new_map_y = location.chunk.map_y - 1;
        new_y += CHUNK_SIZE;
    }
    if (new_x >= CHUNK_SIZE)
    {
        new_map_x++;
        new_x -= CHUNK_SIZE;
    }
    if (new_y >= CHUNK_SIZE)
    {
        new_map_y++;
        new_y -= CHUNK_SIZE;
    }

    if (new_map_x != location.chunk.map_x || new_map_y != location.chunk.map_y)
    {
        if (!check_and_load_chunk(new_map_x, new_map_y))
            return;
    }

    location.chunk.x = new_x;
    location.chunk.y = new_y;

    if (old.chunk.map_x != new_map_x || old.chunk.map_y != new_map_y)
    {
        remove_from_chunks(this);
        location.chunk.map_x = new_map_x;
        location.chunk.map_y = new_map_y;
        add_object_to_world(this, this->location);
    }

    update_location(NetworkObject(get_cid(), get_uid()), old, location);
    // CONSOLE_LOG("SERV: player moved [%d,%d][%d,%d]\n", new_map_x, new_map_y, new_x, new_y);
}

bool PlayerServer::use_item_on_object(InventoryElement * item, InventoryElement * object)
{
    ProductServer * prod = dynamic_cast<ProductServer *>(item);
    if (prod)
    {
        CONSOLE_LOG("%s: using %s on %s\n", get_name(), prod->get_name(), object->get_name());
        if (prod->use_on(object, this))
        {
            return true;
        }
    }
    else if (object->get_cid() == Class_Product)
    {
        ProductServer * obj_prod = dynamic_cast<ProductServer *>(object);
        if (obj_prod)
        {
            CONSOLE_LOG("%s: using %s on %s\n", get_name(), item->get_name(), obj_prod->get_name());
            if (obj_prod->use_on(item, this))
            {
                return true;
            }
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
        case SERVER_TRACE_NETWORK:
            trace_network += 1;
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

void NPCServer::show(bool details)
{

    Npc::show(details);
    BeingServer::show(details);
}

PlayerServer * create_player(size_t id)
{
    CONSOLE_LOG("CREATE PLAYER SERVER\n\n");
    return new PlayerServer(id);
}

NPCServer::NPCServer(size_t uid) : Npc(uid, ItemLocation::center())
{
   // CONSOLE_LOG("NPCServer: uid=%ld\n", uid);
    location.chunk.x = random_range(0, CHUNK_SIZE);
    location.chunk.y = random_range(0, CHUNK_SIZE);

    notify_create(this);
}

bool NPCServer::tick()
{
    if (check_move())
    {
        ItemLocation old_location = location;
     //   CONSOLE_LOG("NPCServer::tick: %s\n", get_name());
        being_move(this, &location);

        if (old_location != location)
        {
            update_location(NetworkObject(get_cid(), get_uid()), old_location, location);
        }
    }
    Player::tick();
    return true;
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

void show_npcs()
{
    ListElement * pl_el = npcs->head;
    while (pl_el)
    {
        NPCServer * pl = static_cast<NPCServer *>(pl_el->get_el());
        pl->show();
        pl_el = pl_el->next;
    }
}