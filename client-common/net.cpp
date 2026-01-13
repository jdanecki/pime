#include <stdint.h>
#include <cstring>
//#include "../core/tiles.h"
#include "../core/packets.h"
#include "net.h"
#include "players.h"

ElementsList objects("objects");

NetClient * client;

extern void print_status(int i, const char * format, ...);

class ObjectElement : public ListElement
{
  public:
    ObjectElement(InventoryElement *el): ListElement(el) {}
    bool check(void *what)
    {
        uintptr_t * uid=(uintptr_t*)what;
        return *uid == el.get()->uid;
    }
};

void add_base_elements(PacketElementsList * list, int i)
{
    switch (list->get_c_id()) {
        case Class_BaseElement:
        {
            BaseElement * base_el = &((BaseElement*) list->get_data())[i];
            base_elements.add(new BaseListElement(new BaseElement(*base_el)));
            break;
        }
        case Class_BasePlant:
        {
            BasePlant * base_el = &((BasePlant*) list->get_data())[i];
            base_plants.add(new BaseListElement(new BasePlant(*base_el)));
            break;
        }
        case Class_BaseAnimal:
        {
            BaseAnimal * base_el = &((BaseAnimal*) list->get_data())[i];
            base_animals.add(new BaseListElement(new BaseAnimal(*base_el)));
            break;
        }
    }
}

 extern void update_hotbar();
void add_elements(PacketElementsList * list, int i)
{
    size_t uid= ((size_t*) list->get_data())[i];
    Player * p = (Player *)get_object_by_id(NetworkObject(Class_Player, list->get_pl_id()));
    if (p)
    {
       InventoryElement * el = get_object_by_id(NetworkObject(Class_Element, uid));
       if (el) p->pickup(el);
        printf("player=%s [%d]=%lx inv.elements=%d\n", p->get_name(), i, uid, p->inventory.nr_elements);
    }
    update_hotbar();
}

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

void send_packet_request_item(NetClient * client, size_t id)
{
    Packet * p = new PacketRequestItem(id);
    p->send(client->peer);
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


bool handle_packet(ENetPacket * packet)
{
    bool ret = false;
    unsigned char * data = packet->data;
  //  printf("Received length=%lu: %d\n", packet->dataLength, *data);

    Packet * p = check_packet('R', data, packet->dataLength);
    if (!p)
        return ret;

    switch (p->get_type())
    {
        case PACKET_ACTION_FAILED:
        {
            printf("action failed\n");
            ret = true;
            break;
        }
        case PACKET_PLAYER_ID:
        {
            PacketPlayerId * id = dynamic_cast<PacketPlayerId *>(p);
            got_id(id->get_id(), 0);
            ret = true;
            break;
        }            
        case PACKET_CHUNK_UPDATE:
        {
            PacketChunkUpdate * up = dynamic_cast<PacketChunkUpdate *>(p);
            update_chunk(up->get_x(), up->get_y(), up->get_table());
            ret = true;
            break;
        }
        case PACKET_OBJECT_CREATE:
        {
            PacketObjectCreate * obj = dynamic_cast<PacketObjectCreate *>(p);

            create_object(obj->obj);
            ret = true;
            break;
        }
        case PACKET_OBJECT_UPDATE:
        {
            PacketObjectUpdate * obj = dynamic_cast<PacketObjectUpdate *>(p);

            update_object(obj->obj);
            ret = true;
            break;
        }
        case PACKET_OBJECT_DESTROY:
        {
            PacketObjectDestroy * obj = dynamic_cast<PacketObjectDestroy *>(p);

            destroy_object(NetworkObject(Class_Unknown, obj->get_id()), obj->get_location());
            ret = true;
            break;
        }
        case PACKET_LOCATION_UPDATE:
        {
            PacketLocationUpdate * loc = dynamic_cast<PacketLocationUpdate *>(p);
            update_item_location(loc->get_location());
            ret = true;
            break;
        }
        case PACKET_KNOWLEDGE_UPDATE:
        {
            PacketKnowledgeUpdate * upd = dynamic_cast<PacketKnowledgeUpdate *>(p);
            knowledge_update(upd->get_pl_id(), upd->get_cid(), upd->get_id());
            ret = true;
            break;
        }
        case PACKET_CHECKED_UPDATE:
        {
            PacketCheckedUpdate * upd = dynamic_cast<PacketCheckedUpdate *>(p);
            checked_update(upd->get_pl_id(), upd->get_id());
            ret = true;
            break;
        }
        case PACKET_ELEMENTS_LIST:
        {
            PacketElementsList * list = dynamic_cast<PacketElementsList *>(p);
            for (int i=0; i < list->get_nr_elements(); i++)
            {
                switch(list->get_list_c_id())
                {
                    case Class_BaseListElement:
                        add_base_elements(list, i);
                        break;
                    case Class_ListElement:
                        add_elements(list, i);
                        break;
                }


            }
            ret = true;
            break;
        }

    }
    delete p;
    return ret;
}

NetClient * init(const char * server_ip, const char * port)
{
    //trace_network = 1;

    if (enet_initialize() != 0)
    {
        fprintf(stderr, "Enet initialization failed\n");
        return nullptr;
    }
    atexit(enet_deinitialize);

    ENetHost * host = enet_host_create(NULL, 1, 2, 0, 0);
    if (!host)
    {
        fprintf(stderr, "Can't create client\n");
        return nullptr;
    }

    ENetAddress address;
    enet_address_set_host(&address, server_ip);
    address.port = atoi(port);

    ENetPeer * peer = enet_host_connect(host, &address, 2, 0);
    if (!peer)
    {
        fprintf(stderr, "Can't connect to server\n");
        return nullptr;
    }

    ENetEvent event;

    if (enet_host_service(host, &event, 5000) > 0 // 5000 ms
        && event.type == ENET_EVENT_TYPE_CONNECT)
    {

        char hostname[256];
        enet_address_get_host_ip(&event.peer->address, hostname, 256);
        printf("Connected to %s:%d\n", hostname, event.peer->address.port);
    }
    else
    {
        enet_peer_reset(peer);
        printf("Can't connect to server\n");
        return nullptr;
    }

    Packet * p = new PacketJoinRequest();
    p->send(peer);    

    if (enet_host_service(host, &event, 5000) > 0 && event.type == ENET_EVENT_TYPE_RECEIVE)
    {
        handle_packet(event.packet);
        enet_packet_destroy(event.packet);
        return new NetClient(host, peer);
    }
    else
    {
        printf("Server did not answer\n");

        enet_peer_disconnect(peer, 0);

        while (enet_host_service(host, &event, 3000) > 0)
        {
            if (event.type == ENET_EVENT_TYPE_DISCONNECT)
            {
                printf("Disconnected from server\n");
                break;
            }
        }
        return nullptr;
    }
}

unsigned int network_tick(NetClient * client)
{
    ENetEvent event;
    unsigned int recv = 0;
    while (enet_host_service(client->host, &event, 10) > 0)
    {
        switch (event.type)
        {
            case ENET_EVENT_TYPE_RECEIVE:
            {
                recv += event.packet->dataLength;
                handle_packet(event.packet);
                enet_packet_destroy(event.packet);
                break;
            }
            default:
                break;
        }
    }

    return recv;
}

InventoryElement * get_object_by_id(NetworkObject uid)
{
    ListElement *el = objects.find(&uid.uid);
    return el ? static_cast<InventoryElement*>(el->el.get()) : nullptr;
}

void register_object(NetworkObject * o)
{
    ObjectElement *obj = new ObjectElement((InventoryElement*)o);
   // printf("register_object: uid=%lx\n", o->uid);
    objects.add(obj);
}

void deregister_object(NetworkObject * o)
{
    ListElement * obj= objects.find(&o->uid);
    objects.remove(obj);
}

Base * get_base(uint32_t c_id, int32_t id)
{
    return nullptr;
   // return base.find(&id);
}

void send_packet_move(NetClient * client, int32_t x, int32_t y)
{
    Packet * p = new PacketPlayerMove(x, y);
    p->send(client->peer);
}

void send_packet_pickup(NetClient * client, uintptr_t id)
{
    Packet * p = new PacketPlayerActionPickup(id);
    p->send(client->peer);
}

void send_packet_drop(NetClient * client, uintptr_t id)
{
    Packet * p = new PacketPlayerActionDrop(id);
    p->send(client->peer);
}

void send_packet_item_used_on_object(NetClient * client, uintptr_t iid, uintptr_t oid)
{
    Packet * p = new PacketPlayerActionUseItemOnObject(iid, oid);
    p->send(client->peer);
}

void send_packet_action_on_object(NetClient * client, int32_t a, uintptr_t oid)
{
    Packet * p = new PacketPlayerActionOnObject((Player_action)a, oid);
    p->send(client->peer);
}

void send_packet_server_action_on_object(NetClient * client, int32_t a, uintptr_t oid)
{
    Packet * p = new PacketServerActionOnObject((Server_action)a, oid);
    p->send(client->peer);
}

void send_packet_item_used_on_tile(NetClient * client, uintptr_t iid, ItemLocation location)
{
    if (location.tag == ItemLocation::Tag::Chunk)
    {
        Packet * p = new PacketPlayerActionUseItemOnTile(iid, location.chunk.map_x, location.chunk.map_y, location.chunk.x, location.chunk.y);
        p->send(client->peer);
    }
}

void send_packet_craft(NetClient * client, uintptr_t prod_id, uintptr_t ingredients_num, const uintptr_t * iid)
{
    Packet * p = new PacketPlayerActionCraft(prod_id, ingredients_num, iid);
    p->send(client->peer);
}

void send_packet_request_chunk(NetClient * client, int32_t cx, int32_t cy)
{
    Packet * p = new PacketRequestChunk(cx, cy);
    p->send(client->peer);
}

void server_action_tile(Server_action a, ItemLocation loc)
{
    InventoryElement * object = get_item_at(loc);
    if (object)
    {
        CONSOLE_LOG("Client: server action %s on %s\n", server_action_name[a], object->get_name());
        send_packet_server_action_on_object(client, a, object->uid);
    }
    else
    {
        switch (a)
        {
            case SERVER_SHOW_CHUNK:
            case SERVER_TRACE_NETWORK:
                CONSOLE_LOG("Client: server action %s\n", server_action_name[a]);
                send_packet_server_action_on_object(client, a, 0);
                break;
            default:
                CONSOLE_LOG("Client: nothing to show\n");
                break;
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

void add_packet_to_send1(Packet * p) {}


