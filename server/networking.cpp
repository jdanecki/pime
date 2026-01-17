#include "networking.h"
#include "elements_server.h"
#include "craft.h"
#include "world_server.h"

ElementsList objects_to_create;
ENetHost * server;
int players_id;

ElementsList *packets_to_send, *packets_to_send1;

Networked::Networked()
{
    notify_create((InventoryElement *)this);
}

// #define SEND_LOG=1
void send_to_all(Packet * p)
{
    ListElement * pl_el = players->head;
#ifdef SEND_LOG
    int i = 0;
#endif
    while (pl_el)
    {
        PlayerClient * pl = (PlayerClient *)pl_el;
#ifdef SEND_LOG
        CONSOLE_LOG("[%d/%d] sending to player: %d\n", i++, players->nr_elements, pl->player->get_id());
#endif
        p->send(pl->peer);
        pl_el = pl_el->next;
    }
}

class PacketToSend : public ListElement
{
    Packet * p;

  public:
    PacketToSend(Packet * p) : p(p)
    {
    }
    void send(ENetPeer * peer)
    {
        p->send(peer);
    }
    void to_all()
    {
        send_to_all(p);
    }
};

bool handle_packet(ENetPacket * packet, ENetPeer * peer)
{
    unsigned char * data = packet->data;
    // CONSOLE_LOG("Received length=%lu: %d\n", packet->dataLength, *data);

    Packet * p = check_packet('R', data, packet->dataLength);
    if (!p)
        return false;

    Peer_id peer_id;
    peer_id.tag = Peer_id::Tag::Peer;
    peer_id.peer = peer;
    PlayerClient * pl = nullptr;
    if (players->nr_elements)
        pl = (PlayerClient *)players->find(&peer_id);

    switch (p->get_type())
    {
        case PACKET_JOIN_REQUEST:
        {
            delete p;
            p = new PacketPlayerId(players_id++);
            p->send(peer);
            PlayerClient * new_pl = new PlayerClient(new PlayerServer(((PacketPlayerId *)p)->get_id()), peer);
            players->add(new_pl);
            delete p;
            p = new PacketElementsList(&base_elements);
            p->send(peer);
            delete p;
            p = new PacketElementsList(&base_plants);
            p->send(peer);
            delete p;
            p = new PacketElementsList(&base_animals);
            p->send(peer);
            delete p;

            p = new PacketChunkUpdate(128, 128);
            p->send(peer);
            delete p;
            add_object_to_world(new_pl->player, new_pl->player->location);
            break;
        }
        case PACKET_PLAYER_MOVE:
        {
            PacketPlayerMove * move = static_cast<PacketPlayerMove *>(p);
            pl->player->move(move->get_x(), move->get_y());
            delete p;
            break;
        }
        case PACKET_REQUEST_CHUNK:
        {
            PacketRequestChunk * req = static_cast<PacketRequestChunk *>(p);
            int cx = req->get_cx();
            int cy = req->get_cy();
            delete p;
            p = new PacketChunkUpdate(cx, cy);
            p->send(peer);
            delete p;
            break;
        }
        case PACKET_PLAYER_ACTION_PICKUP:
        {
            PacketPlayerActionPickup * req = static_cast<PacketPlayerActionPickup *>(p);
            uintptr_t id = req->get_id();
            InventoryElement * el = find_in_world(&pl->player->location, id);
            delete p;
            if (el)
            {
                if (!pl->player->pickup(el))
                {
                    p = new PacketActionFailed();
                    p->send(peer);
                    delete p;
                }
            }
            break;
        }
        case PACKET_PLAYER_ACTION_DROP:
        {
            PacketPlayerActionDrop * req = static_cast<PacketPlayerActionDrop *>(p);
            uintptr_t id = req->get_id();
            InventoryElement * el = pl->player->get_item_by_uid(id);
            delete p;
            if (el)
            {
                ItemLocation location = el->location;
                add_object_to_world(el, pl->player->location);
                pl->player->drop(el);
                update_location(*el, location, el->location);
            }
            break;
        }
        case PACKET_PLAYER_ACTION_USE_ITEM_ON_OBJECT:
        {
            PacketPlayerActionUseItemOnObject * req = static_cast<PacketPlayerActionUseItemOnObject *>(p);
            uintptr_t iid = req->get_iid();
            uintptr_t oid = req->get_oid();
            InventoryElement * el = pl->player->get_item_by_uid(iid);
            delete p;
            if (el)
            {
                InventoryElement * obj = find_in_world(&pl->player->location, oid);
                CONSOLE_LOG("use item on object: oid=%lx obj=%p\n", oid, obj);
                if (obj && !pl->player->use_item_on_object(el, obj))
                {
                    p = new PacketActionFailed();
                    p->send(peer);
                    delete p;
                }
            }
            break;
        }
        case PACKET_PLAYER_ACTION_ON_OBJECT:
        {
            PacketPlayerActionOnObject * req = static_cast<PacketPlayerActionOnObject *>(p);
            Player_action a = req->get_a();
            uintptr_t oid = req->get_oid();
            delete p;
            InventoryElement * obj = find_in_world(&pl->player->location, oid);
            if (obj)
            {
                if (!pl->player->action_on_object(a, obj))
                {
                    p = new PacketActionFailed();
                    p->send(peer);
                    delete p;
                }
            }
            break;
        }
        case PACKET_SERVER_ACTION_ON_OBJECT:
        {
            PacketServerActionOnObject * req = static_cast<PacketServerActionOnObject *>(p);
            Server_action a = req->get_a();
            uintptr_t oid = req->get_oid();
            delete p;
            InventoryElement * obj = find_in_world(&pl->player->location, oid);
            if (obj)
            {
                if (!pl->player->server_action_on_object(a, obj))
                {
                    p = new PacketActionFailed();
                    p->send(peer);
                    delete p;
                }
            }
            break;
        }
        case PACKET_PLAYER_ACTION_USE_ITEM_ON_TILE:
        {
            PacketPlayerActionUseItemOnTile * req = static_cast<PacketPlayerActionUseItemOnTile *>(p);
            int map_x = req->get_map_x();
            int map_y = req->get_map_y();
            int x = req->get_x();
            int y = req->get_y();
            uintptr_t iid = req->get_iid();
            delete p;
            InventoryElement * item = pl->player->get_item_by_uid(iid);
            if (item)
            {
                if (Product * prod = static_cast<Product *>(item))
                {
                    CONSOLE_LOG("use product: %s on tile [%d,%d]:[%d,%d]\n", prod->get_name(), map_x, map_y, x, y);
                    if (!pl->player->use_product_on_tile(prod, map_x, map_y, x, y))
                    {
                        p = new PacketActionFailed();
                        p->send(peer);
                        delete p;
                    }
                }
                else
                {
                    CONSOLE_LOG("use item: %s on tile [%d,%d]:[%d,%d]\n", item->get_name(), map_x, map_y, x, y);
                    /*if (!pl->player->plant_with_seed(item, map_x, map_y, x, y))
                    {
                        p = new PacketActionFailed();
                        p->send(peer);
                        delete p;
                    }*/
                }
            }
            break;
        }
        case PACKET_PLAYER_ACTION_CRAFT:
        {
            PacketPlayerActionCraft * req = static_cast<PacketPlayerActionCraft *>(p);
            uintptr_t prod_id = req->get_prod_id();
            uintptr_t ing_num = req->get_ing_num();
            uintptr_t * iid_table = req->get_iid_table();
            delete p;
            if (craft_entry(prod_id, ing_num, iid_table, pl->player))
            {
                CONSOLE_LOG("crafted id=%ld\n", prod_id);
                InventoryElement * el1 = pl->player->get_item_by_uid(iid_table[0]);
                if (el1)
                {
                    p = new PacketObjectDestroy(el1->uid, el1->location);
                    p->send(peer);
                    delete p;

                    pl->player->drop(el1);
                }
                if (prod_id >= ING_COUNT)
                {
                    InventoryElement * el2 = pl->player->get_item_by_uid(iid_table[1]);
                    if (el2)
                    {
                        p = new PacketObjectDestroy(el2->uid, el2->location);
                        p->send(peer);
                        pl->player->drop(el2);
                    }
                }
            }
            else
            {
                p = new PacketActionFailed();
                p->send(peer);
                delete p;
            }
            break;
        }
    }
    return true;
}

void send_updates()
{
    if (!players->nr_elements)
        return;

    if (packets_to_send->nr_elements)
    {
        ListElement * el = packets_to_send->head;
        //        CONSOLE_LOG("sending updates elems=%d\n", packets_to_send->nr_elements);
        while (el)
        {
            PacketToSend * p = (PacketToSend *)el;
            p->to_all();
            el = el->next;
        }
        //        CONSOLE_LOG("sent updates\n");
    }

    if (packets_to_send1->nr_elements)
    {
        ListElement * el = packets_to_send1->head;
        // CONSOLE_LOG("sending updates1 elems=%d\n", packets_to_send1->nr_elements);
        while (el)
        {
            PacketToSend * p = (PacketToSend *)el;
            p->to_all();
            el = el->next;
        }
        //  CONSOLE_LOG("sent updates1\n");
    }

    if (objects_to_create.nr_elements)
    {
        ListElement * el = objects_to_create.head;
        while (el)
        {
            //            CONSOLE_LOG("sending objects to create: %s id=%lx\n", static_cast<InventoryElement*> (el->el.get())->get_name(), el->el.get()->uid);
            Packet * p = new PacketObjectCreate(el->get_el());
            send_to_all(p);
            el = el->next;
        }
    }

    packets_to_send->remove_all();
    packets_to_send1->remove_all();
    objects_to_create.remove_all();
}

void add_packet_to_send(Packet * p)
{
    packets_to_send->add(new PacketToSend(p));
}

void add_packet_to_send1(Packet * p)
{
    packets_to_send1->add(new PacketToSend(p));
}

void notify_update(const InventoryElement * el)
{
    add_packet_to_send(new PacketObjectUpdate((InventoryElement *)el));
}

void notify_create(const InventoryElement * el)
{
    // add_packet_to_send(new PacketObjectCreate((InventoryElement *)el));
    objects_to_create.add((InventoryElement *)el);
}

void update_location(NetworkObject id, ItemLocation old_loc, ItemLocation new_loc)
{
    /*CONSOLE_LOG("update location uid=%lx old_tag=%d new_tag=%d\n", id, (int)old_loc.tag, (int)new_loc.tag);
     old_loc.show();
     new_loc.show();
 */
    add_packet_to_send(new PacketLocationUpdate(id.uid, old_loc, new_loc));
}
void notify_destroy(NetworkObject id, ItemLocation location)
{
    add_packet_to_send(new PacketObjectDestroy(id.uid, location));
}
void notify_knowledge(size_t pl_id, Class_id cid, int id)
{
    add_packet_to_send(new PacketKnowledgeUpdate(pl_id, cid, id));
}
void notify_checked(size_t pl_id, size_t el)
{
    add_packet_to_send(new PacketCheckedUpdate(pl_id, el));
}

bool init_networking()
{
    if (enet_initialize() != 0)
    {
        fprintf(stderr, "Can't initialize ENet\n");
        return false;
    }
    atexit(enet_deinitialize);

    ENetAddress address;
    address.host = ENET_HOST_ANY;
    address.port = 1234;

    server = enet_host_create(&address, 32, 2, 0, 0);
    // 32 clients, 2 channels, bandwidth in, bandwidth out
    // 0-255 channels
    if (!server)
    {
        fprintf(stderr, "Can't create host\n");
        return false;
    }
    packets_to_send = new ElementsList("packets to send");
    packets_to_send1 = new ElementsList("packets1 to send");

    CONSOLE_LOG("Server Pime started on port %u\n", address.port);
    return true;
}

char hostname[512] = {
    0,
};

void handle_net_event(ENetEvent * event)
{
    switch (event->type)
    {
        case ENET_EVENT_TYPE_CONNECT:
        {
            enet_address_get_host_ip(&event->peer->address, hostname, 512);
            CONSOLE_LOG("Client connected %s:%d\n", hostname, event->peer->address.port);
            unsigned short * port = new unsigned short;
            *port = event->peer->address.port;
            event->peer->data = (void *)port;
#if DEBUG_TIMEOUT
            enet_peer_timeout(event->peer, 5 * 60 * 1000, 5 * 60 * 1000, 5 * 60 * 1000); // 5 minutes
#endif
            break;
        }
        case ENET_EVENT_TYPE_RECEIVE:
        {
            handle_packet(event->packet, event->peer);
            enet_packet_destroy(event->packet);
            send_updates();
            break;
        }
        case ENET_EVENT_TYPE_DISCONNECT:
        {
            Peer_id peer_id;
            peer_id.tag = Peer_id::Tag::Peer;
            peer_id.peer = event->peer;
            PlayerClient * pl = (PlayerClient *)players->find(&peer_id);
            enet_address_get_host_ip(&event->peer->address, hostname, 512);

            CONSOLE_LOG("player: %s from %s:%d disconnected\n", pl->player->get_name(), hostname, event->peer->address.port);
            destroy(pl->player);
            players->remove(pl);
            send_updates();
            event->peer->data = NULL;
            break;
        }
        default:
            // CONSOLE_LOG("time=%ld\n", get_time_ms());
            update();
            send_updates();
            break;
    }
}
