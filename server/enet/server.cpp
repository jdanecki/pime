#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include "server.h"
#include "../../core/alchemist/el_list.h"
#include "../cpp-src/player_server.h"
#include "../cpp-src/elements_server.h"
#include "../cpp-src/craft.h"
#include "../cpp-src/networking.h"
#include <math.h>

struct Peer_id
{
    enum class Tag
    {
        Peer,
        Id,
    };
    Tag tag;
    union {
        ENetPeer *peer;
        unsigned long id;
    };
};

class PacketToSend : public ListElement
{
    Packet *p;
    public:
    PacketToSend(Packet *p): p(p) {}
    void send(ENetPeer * peer) {
        p->send(peer);
    }
};

ElementsList * players, * packets_to_send;
ENetHost * server;

class PlayerClient : public ListElement
{
  public:
    PlayerServer * player;
    ENetPeer *peer;
    PlayerClient(PlayerServer *player, ENetPeer * peer): player(player), peer(peer)
    {
        char hostname[64];
        enet_address_get_host_ip(&peer->address, hostname, 64);
        printf("new player uid=%ld host=%s port=%u\n",player->uid, hostname, peer->address.port);
    }
    bool check(void * what)
    {
        Peer_id * p=(Peer_id*) what;
        switch(p->tag)
        {
            case Peer_id::Tag::Peer:  return p->peer == peer;
            case Peer_id::Tag::Id:    return p->id == player->uid;
        }
        return false;
    }

};

void print_status(int l, const char * format, ...)
{
    va_list args;
    va_start(args, format);
    if (!l)
        vprintf(format, args);
    else
        vprintf(format, args);
    va_end(args);
    puts("");
}

void send_list(ElementsList * list, ENetPeer *peer)
{
    if (!list->nr_elements) return;
    ListElement * el = list->head;
    printf("sending from list: %s\n", list->name);
    while (el)
    {
        PacketToSend * p = (PacketToSend*) el;
        p->send(peer);
        el = el->next;
    }
    list->remove_all();
}


bool handle_packet(ENetPacket * packet, ENetPeer * peer)
{
    unsigned char * data = packet->data;
    printf("Received length=%lu: %d\n", packet->dataLength, *data);

    Packet * p = check_packet('R', data, packet->dataLength);
    if (!p)
        return false;

    switch (p->get_type())
    {
        case PACKET_JOIN_REQUEST:
        {
            delete p;
            p = new PacketPlayerId(players->nr_elements);
            p->send(peer);            
            PlayerClient *pl = new PlayerClient(new PlayerServer(players->nr_elements), peer);
            players->add(pl);            
            delete p;
            p = new PacketChunkUpdate(128, 128);
            p->send(peer);            
            delete p;
            add_object_to_world(pl->player, pl->player->location);
            break;
        }
        case PACKET_PLAYER_MOVE:
        {
            PacketPlayerMove * move = dynamic_cast<PacketPlayerMove *>(p);
            Peer_id peer_id;
            peer_id.tag = Peer_id::Tag::Peer;
            peer_id.peer = peer;
            PlayerClient *pl=(PlayerClient*) players->find(&peer_id);
            pl->player->move(move->get_x(), move->get_y());
            delete p;            
            break;
        }
        case PACKET_REQUEST_CHUNK:
        {
            PacketRequestChunk * req = dynamic_cast<PacketRequestChunk *>(p);
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
            PacketPlayerActionPickup * req = dynamic_cast<PacketPlayerActionPickup *>(p);
            uintptr_t id = req->get_id();
            Peer_id peer_id;
            peer_id.tag = Peer_id::Tag::Peer;
            peer_id.peer = peer;
            PlayerClient *pl=(PlayerClient*) players->find(&peer_id);

            InventoryElement *el = find_in_world(&pl->player->location, id);
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
            PacketPlayerActionDrop * req = dynamic_cast<PacketPlayerActionDrop *>(p);
            uintptr_t id = req->get_id();
            Peer_id peer_id;
            peer_id.tag = Peer_id::Tag::Peer;
            peer_id.peer = peer;
            PlayerClient *pl=(PlayerClient*) players->find(&peer_id);

            InventoryElement *el = pl->player->get_item_by_uid(id);
            delete p;
            if (el)
            {
                ItemLocation location = el->location;
                add_object_to_world(el, pl->player->location);
                pl->player->drop(el);
                update_location(el->uid, location, el->location);
            }
            break;
        }
        case PACKET_PLAYER_ACTION_USE_ITEM_ON_OBJECT:
        {
            PacketPlayerActionUseItemOnObject * req = dynamic_cast<PacketPlayerActionUseItemOnObject*>(p);
            uintptr_t iid = req->get_iid();
            uintptr_t oid = req->get_oid();
            Peer_id peer_id;
            peer_id.tag = Peer_id::Tag::Peer;
            peer_id.peer = peer;
            PlayerClient *pl=(PlayerClient*) players->find(&peer_id);

            InventoryElement *el = pl->player->get_item_by_uid(iid);
            delete p;
            if (el)
            {
                InventoryElement *obj = find_in_world(&pl->player->location, oid);
                if (!pl->player->use_item_on_object(el, obj))
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
            PacketPlayerActionOnObject * req = dynamic_cast<PacketPlayerActionOnObject*>(p);
            Player_action a = req->get_a();
            uintptr_t oid = req->get_oid();
            Peer_id peer_id;
            peer_id.tag = Peer_id::Tag::Peer;
            peer_id.peer = peer;
            PlayerClient *pl=(PlayerClient*) players->find(&peer_id);
            delete p;
            InventoryElement *obj = find_in_world(&pl->player->location, oid);
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
            PacketServerActionOnObject * req = dynamic_cast<PacketServerActionOnObject*>(p);
            Server_action a = req->get_a();
            uintptr_t oid = req->get_oid();
            Peer_id peer_id;
            peer_id.tag = Peer_id::Tag::Peer;
            peer_id.peer = peer;
            PlayerClient *pl=(PlayerClient*) players->find(&peer_id);
            delete p;
            InventoryElement *obj = find_in_world(&pl->player->location, oid);
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
            PacketPlayerActionUseItemOnTile * req = dynamic_cast<PacketPlayerActionUseItemOnTile*>(p);
            int map_x = req->get_map_x();
            int map_y = req->get_map_y();
            int x = req->get_x();
            int y = req->get_y();
            uintptr_t iid = req->get_iid();
            Peer_id peer_id;
            peer_id.tag = Peer_id::Tag::Peer;
            peer_id.peer = peer;
            PlayerClient *pl=(PlayerClient*) players->find(&peer_id);
            delete p;
            InventoryElement *item = pl->player->get_item_by_uid(iid);
            if (item)
            {
                if (!pl->player->plant_with_seed(item, map_x, map_y, x, y))
                {
                    p = new PacketActionFailed();
                    p->send(peer);
                    delete p;
                }
            }
            break;
        }
        case PACKET_PLAYER_ACTION_CRAFT:
        {
            PacketPlayerActionCraft * req = dynamic_cast<PacketPlayerActionCraft*>(p);
            uintptr_t prod_id = req->get_prod_id();
            uintptr_t ing_num = req->get_ing_num();
            uintptr_t *iid_table = req->get_iid_table();
            Peer_id peer_id;
            peer_id.tag = Peer_id::Tag::Peer;
            peer_id.peer = peer;
            PlayerClient *pl=(PlayerClient*) players->find(&peer_id);
            delete p;
            if (craft_entry(prod_id, ing_num, iid_table,pl->player))
            {
                printf("crafted id=%ld\n", prod_id);
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
    send_list(packets_to_send, peer);

    if (objects_to_create.nr_elements)
    {
        ListElement * el = objects_to_create.head;
        while (el)
        {
            printf("objects_to_create: %s id=%lx\n", el->el->get_name(), el->el->uid);
            Packet * p = new PacketObjectCreate(el->el);
            p->send(peer);
            el = el->next;
        }
        objects_to_create.remove_all();
    }
    return true;
}
extern "C"
{

    void load_chunk(int cx, int cy)
    {
        printf("load_chunk(%d, %d)\n", cx, cy);
        chunk * ch = new chunk(cx, cy);

        for (int y = 0; y < CHUNK_SIZE; y++)
            for (int x = 0; x < CHUNK_SIZE; x++)
                ch->table[y][x].tile = cx;

        ElementServer * el = create_element(new BaseElement(Form_solid, rand() % 20));
      //  el->show(true);
        ch->add_object(el);
        world_table[cy][cx] = ch;
    }

    void notify_update(const InventoryElement * el)
    {
        packets_to_send->add(new PacketToSend(new PacketObjectUpdate((InventoryElement*) el)));
    }

    void update_location(size_t id, ItemLocation old_loc, ItemLocation new_loc)
    {
        printf("update location uid=%lx old_tag=%d new_tag=%d\n", id, (int)old_loc.tag, (int)new_loc.tag);
        old_loc.show();
        new_loc.show();

        packets_to_send->add(new PacketToSend(new PacketLocationUpdate(id, old_loc, new_loc)));
    }
    void notify_destroy(size_t id, ItemLocation location)
    {
        packets_to_send->add(new PacketToSend(new PacketObjectDestroy(id, location)));
    }
    void notify_knowledge(size_t pl_id, Class_id cid, int id)
    {
        packets_to_send->add(new PacketToSend(new PacketKnowledgeUpdate(pl_id, cid, id)));
    }
    void notify_checked(size_t pl_id, size_t el)
    {
        packets_to_send->add(new PacketToSend(new PacketCheckedUpdate(pl_id, el)));
    }
}
int main()
{
    trace_network = 1;

    if (enet_initialize() != 0)
    {
        fprintf(stderr, "Błąd inicjalizacji ENet\n");
        return 1;
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
        fprintf(stderr, "Nie udało się utworzyć hosta serwera\n");
        return 1;
    }
    printf("Server Pime started on port %u\n", address.port);

    players = new InvList("Players");
    for (int cy=127; cy < 130; cy++)
        for (int cx=127; cx < 130; cx++)
            load_chunk(cy, cx);

    packets_to_send = new ElementsList("packets to send");

    ENetEvent event;
    char hostname[512] = {
        0,
    };

    while (enet_host_service(server, &event, 1000) >= 0)
    {
        switch (event.type)
        {
            case ENET_EVENT_TYPE_CONNECT:
            {
                enet_address_get_host_ip(&event.peer->address, hostname, 512);
                printf("Client connected %s:%d\n", hostname, event.peer->address.port);
                unsigned short * port = new unsigned short;
                *port = event.peer->address.port;
                event.peer->data = (void *)port;
            }
            break;

            case ENET_EVENT_TYPE_RECEIVE:
            {
                handle_packet(event.packet, event.peer);
                enet_packet_destroy(event.packet);
            }
            break;

            case ENET_EVENT_TYPE_DISCONNECT:
                printf("Client [%d] disconnected ", *(unsigned short *)(event.peer->data));
                enet_address_get_host_ip(&event.peer->address, hostname, 512);
                printf("%s:%d\n", hostname, event.peer->address.port);
                event.peer->data = NULL;
                break;

            default:
                break;
        }
    }

    enet_host_destroy(server);
    return 0;
}
