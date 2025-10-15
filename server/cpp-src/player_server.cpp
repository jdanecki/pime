#include "player_server.h"
#include "elements_server.h"
#include "networking.h"
#include "world_server.h"
#include "../../core/packet_types.h"
#include <stdio.h>

bool check_and_load_chunk(int new_map_x, int new_map_y)
{
    for (int cy = new_map_y - 1; cy <= new_map_y + 1; cy++)
    {
        if (cy >= 0 && cy < WORLD_SIZE)
        {
            for (int cx = new_map_x - 1; cx <= new_map_x + 1; cx++)
                if (cx >= 0 && cx < WORLD_SIZE)
                {
                    if (!world_table[cy][cx])
                        load_chunk(cx, cy);
                }
        }
    }
    return true;
}

void PlayerServer::move(int dx, int dy)
{
    ItemLocation old = location;
    // TODO cleanup
    int new_x = location.chunk.x + dx;
    int new_y = location.chunk.y + dy;
    int new_map_x = location.chunk.map_x;
    int new_map_y = location.chunk.map_y;

 //   CONSOLE_LOG("SERV: player move dx=%d dy=%d\n", dx, dy);

    if (!((new_x >= 0 && new_x < CHUNK_SIZE) && (new_y >= 0 && new_y < CHUNK_SIZE)))
    {
        if (new_x < 0 || new_x >= CHUNK_SIZE)
        {
            new_map_x += dx;
            new_x += -CHUNK_SIZE * dx;
            if (!check_and_load_chunk(new_map_x, new_map_y))
                return;
            goto move_player;
        }

        if (new_y < 0 || new_y >= CHUNK_SIZE)
        {
            new_map_y += dy;
            new_y += -CHUNK_SIZE * dy;
            if (!check_and_load_chunk(new_map_x, new_map_y))
                return;
        }
    }
move_player:
    location.chunk.x = new_x;
    location.chunk.y = new_y;

    if (old.chunk.map_x != new_map_x || old.chunk.map_y != new_map_y)
    {
        remove_from_chunks(this);
        location.chunk.map_x = new_map_x;
        location.chunk.map_y = new_map_y;
        add_object_to_world(this, this->location);
    }

    hunger--;
    thirst--;
    update_location(get_uid(), old, location);
   // CONSOLE_LOG("SERV: player moved [%d,%d][%d,%d]\n", new_map_x, new_map_y, new_x, new_y);
    notify_update(this);
}

bool PlayerServer::use_item_on_object(InventoryElement * item, InventoryElement * object)
{
    ProductServer * i = dynamic_cast<ProductServer *>(item);
    if (i)
    {
        CONSOLE_LOG("%s: using %s on %s\n", get_name(), i->get_name(), object->get_name());
        return i->use(object, this);
    }
    else
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

bool PlayerServer::plant_with_seed(InventoryElement * el, int map_x, int map_y, int x, int y)
{
    // FIXME
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

            p->phase = Plant_seed;
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
    update_location(item->get_uid(), old_location, item->location);
    return true;
}

PlayerServer::PlayerServer(size_t uid) : Player(uid, SerializableCString("player"), ItemLocation::center(), 50 + rand() % 100, 50 + rand() % 100, 50 + rand() % 100)
{
    CONSOLE_LOG("PlayerServer: uid=%ld\n", uid);
    objects_to_create.add(this);
}

PlayerServer * create_player(size_t id)
{
    return new PlayerServer(id);
}

Npc * create_npc()
{
    return new Npc(ItemLocation::center());
}
