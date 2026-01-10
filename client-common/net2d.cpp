#include "net.h"
#include "players.h"
#include <cstring>

 extern void update_hotbar();
extern void print_status(int i, const char * format, ...);

void knowledge_update(size_t pl_id, Class_id cid, int id)
{
    if (pl_id != player->get_id())
        return;

    CONSOLE_LOG("knowledge update for player %ld cid=%s id=%d\n", pl_id, class_name[cid], id);
    Player * p = players[pl_id];
    if (!p)
        return;
    p->set_known(cid, id);
}

void got_id(size_t id, int64_t seed)
{
    my_id = id;

    player = (PlayerUI *)calloc(sizeof(PlayerUI), 1);
    player->location.chunk.map_x = 128;
    player->location.chunk.map_y = 128;
    player->location.chunk.x = 8;
    player->location.chunk.y = 8;
    player->name=SerializableCString("player");
    CONSOLE_LOG("seed: %ld\n", seed);
    srand(seed);
    init_sentences();
    init_questions();
    init_answers();
    CONSOLE_LOG("got id %ld\n", id);
    print_status(1, "player %ld connected", id);
}

void checked_update(size_t pl_id, size_t el)
{
    if (pl_id != player->get_id())
        return;

    CONSOLE_LOG("checked update for player %ld el=%lx\n", pl_id, el);
    Player * p = players[pl_id];
    if (!p)
        return;
    p->set_checked(el);
}


InventoryElement * remove_from_location(ItemLocation location, NetworkObject id)
{
    InventoryElement * el = (InventoryElement*)get_object_by_id(id);
    if (!el)
        return nullptr;
    switch (location.tag)
    {
        case ItemLocation::Tag::Chunk:
        {
            remove_from_chunks(el);
            break;
        }
        case ItemLocation::Tag::Player:
        {
            Player * p = (Player *)get_object_by_id(NetworkObject(Class_Player, location.player.id));
            if (p)
                p->drop(el);
            if (location.player.id == player->get_id())
            {
                update_hotbar();
            }
        }
    }
    return el;
}

void destroy_object(NetworkObject id, ItemLocation location)
{
    InventoryElement * el = (InventoryElement*)get_object_by_id(id);
    if (el)
    {
        InventoryElement * removed = remove_from_location(location, id);
        if (removed == nullptr)
        {
            abort();
        }
       CONSOLE_LOG("Client: destroy_object %ld", id.uid);
        deregister_object(el);
        delete el;
    }
    // else
    //  item on not loaded chunk
    //    CONSOLE_LOG("Client: deleting inexisting item %ld\n", id);
}

void update_item_location(LocationUpdateData data)
{
    NetworkObject id = data.id;
    ItemLocation & old_loc = data.old;
    ItemLocation & new_loc = data.new_;

/*    CONSOLE_LOG("update item location uid=%lx old_tag=%d new_tag=%d\n", id.uid, (int)old_loc.tag, (int)new_loc.tag);
    old_loc.show();
    new_loc.show();
*/
    InventoryElement * el = remove_from_location(old_loc, id);
    if (!el)
    { // FIXME
        // CONSOLE_LOG("Client: not found item %lu to remove on chunk [%d,%d][%d,%d]->[%d,%d][%d,%d]\n",
        //      id,
        //      old_loc.chunk.map_x, old_loc.chunk.map_y,
        //      old_loc.chunk.x, old_loc.chunk.y,
        //      new_loc.chunk.map_x, new_loc.chunk.map_y,
        //      new_loc.chunk.x, new_loc.chunk.y);
        if (new_loc.tag == ItemLocation::Tag::Chunk
            && new_loc.chunk.map_x == player->location.chunk.map_x
            && new_loc.chunk.map_y == player->location.chunk.map_y)
            send_packet_request_item(client, id.uid);

        return;
    }
    switch (new_loc.tag)
    {
        case ItemLocation::Tag::Chunk:
        {
            /*CONSOLE_LOG("Client: update item location %s:%s on chunk [%d,%d][%d,%d]->[%d,%d][%d,%d]\n",
                el->get_class_name(), el->get_name(),
                old_loc.chunk.map_x, old_loc.chunk.map_y,
                old_loc.chunk.x, old_loc.chunk.y,
                new_loc.chunk.map_x, new_loc.chunk.map_y,
                new_loc.chunk.x, new_loc.chunk.y);
            */

            if (el->get_cid() == Class_Player)
            {
//                printf("my_id=%lx id=%lx\n", my_id, el->get_id());
                if (my_id == el->get_id()) {
                    print_status(0, " ");
                    print_status(1, " ");
                }
            }

            /*ItemLocation old_l;
            ItemLocation new_l;
            old_l.chunk.x = old_loc.chunk.x;
            old_l.chunk.y = old_loc.chunk.y;
            new_l.chunk.x = new_loc.chunk.x;
            new_l.chunk.y = new_loc.chunk.y;*/
            //el->update_item_location(old_l, new_l);
            el->update_item_location(old_loc, new_loc);
            add_object_to_world(el, new_loc);
            break;
        }
        case ItemLocation::Tag::Player:
        {
            Player * p = (Player *)get_object_by_id(NetworkObject(Class_Player, new_loc.player.id));
            if (p)
                p->pickup(el);
            if (new_loc.player.id == player->get_id())
            {
                update_hotbar(); // FIXME - remove only one element
            }
        }
    }
}

chunk * check_chunk(int cx, int cy)
{
    if (cx < 0 || cy < 0 || cx >= WORLD_SIZE || cy >= WORLD_SIZE)
        return nullptr;

    chunk * ch = world_table[cy][cx];
    if (!ch)
    {
        if (loaded_chunks[cy][cx] == CHUNK_NOT_LOADED)
        {
            send_packet_request_chunk(client, cx, cy);
            loaded_chunks[cy][cx] = CHUNK_LOADING;
            return nullptr;
        }
        else
        {
            CONSOLE_LOG("waiting for chunk %d %d\n", cx, cy);
            return nullptr;
        }
    }
    else
    {
        loaded_chunks[cy][cx] = CHUNK_LOADED;
    }
    return ch;
}

void update_chunk(int32_t x, int32_t y, const chunk_table * data)
{
    data = (chunk_table *)((char *)(data));
    if (!world_table[y][x])
    {
        CONSOLE_LOG("SDL: update_chunk new x=%d y=%d\n", x, y);
        // world_table[y][x] = (chunk*)calloc(1, sizeof(chunk));
        // world_table[y][x]->objects = InvList();
        world_table[y][x] = new chunk(x, y);
        memcpy(world_table[y][x]->table, &data[0], CHUNK_SIZE * CHUNK_SIZE * sizeof(int));
        /*CONSOLE_LOG("got %d items\n[", item_num);
        for (int i = 0; i < 1027 + item_num*20; i++)
        {
           CONSOLE_LOG("%d, ", data[i]);
        }
       CONSOLE_LOG("\n");*/
    }
}

void update_object(const ObjectData * data)
{
    //        size_t uid = data.inv_element.data.uid;
    Class_id c_id = data->inv_element.data.c_id;

    InventoryElement * el = get_object_by_id(data->inv_element.data);
    //FIXME why we get el=NULL? -> change this to get_object_by_uid
    //  CONSOLE_LOG("update_object: el=%p chunk[%d,%d]\n", el, data->inv_element.data.location.chunk.map_x, data->inv_element.data.location.chunk.map_y);
    if (el)
    {
       // CONSOLE_LOG("update_object: el->cid=%x c_id=%x\n", el->c_id, c_id);
    }
    if (el && el->c_id == c_id)
    {
        // CONSOLE_LOG("SDL update_object: %s %s\n", class_name[c_id],  el->get_name());
        switch (c_id)
        {
            case Class_Element:
            {
                Element * element = dynamic_cast<Element *>(el);
                *element = data->element.data;
                break;
            }
            case Class_Ingredient:
            {
                Ingredient * ing = dynamic_cast<Ingredient *>(el);
                *ing = data->ingredient.data;
                break;
            }
            case Class_Product:
            {
                Product * prod = dynamic_cast<Product *>(el);
                *prod = data->product.data;
                break;
            }
            case Class_Plant:
            {
                Plant * plant = dynamic_cast<Plant *>(el);
                *plant = data->plant.data;
                // CONSOLE_LOG("%s size=%f\n", plant->get_name(), plant->size);
                break;
            }
            case Class_Animal:
            {
                Animal * animal = dynamic_cast<Animal *>(el);
                *animal = data->animal.data;
                //     CONSOLE_LOG("%s size=%f\n", animal->get_name(), animal->size);
                break;
            }
            case Class_Player:
            {
                Player * player = dynamic_cast<Player *>(el);
                CONSOLE_LOG("update_object: player=%s inv.elements=%d\n", player->get_name(), player->inventory.nr_elements);
                *player = data->player.data;
                CONSOLE_LOG("update_object: -> update: inv.elements=%d\n", player->inventory.nr_elements);
                break;
            }
            default:
                break;
        }
        // CONSOLE_LOG("%s updated\n", el->get_name());
    }
    else
    {
        /*if (el)
            print_status(1, "bad data for update object %ld %d real %d", uid, c_id, el->c_id);
        else
            print_status(1, "non existing object for update object %ld %d", uid, c_id);*/
    }
}

void create_object(const ObjectData * data)
{
    NetworkObject * object = el_from_data(data);
    if (object)
    {
        register_object(object);
        if (object->c_id != Class_Clan)
        {
            InventoryElement* el = (InventoryElement*)object;
            add_object_to_world(el, el->location);
        }
    }
    else
    {
        CONSOLE_LOG("SDL: inexisting chunk\n");
    }
}

void failed_craft()
{
    CONSOLE_LOG("craft FAILED\n");
    print_status(1, "failed craft");
}

void action_failed()
{
    CONSOLE_LOG("action FAILED\n");
    print_status(1, "action failed");
}

