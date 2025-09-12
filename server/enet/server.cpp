#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include "server.h"
#include "../../core/alchemist/el_list.h"
#include "../cpp-src/player_server.h"
#include "../cpp-src/world_server.h"
#include "../cpp-src/craft.h"
#include "../cpp-src/networking.h"
#include <math.h>

ElementsList base_elements("base elements");
ElementsList base_plants("base plants");
ElementsList base_animals("base animals");

BaseElement * get_base_element(int32_t id)
{
    BaseListElement *el=(BaseListElement*)base_elements.find(&id);
    if (!el) return nullptr;
    return (BaseElement*)((el)->base);
}

BasePlant * get_base_plant(int32_t id)
{
    BaseListElement *el=(BaseListElement*)base_plants.find(&id);
    if (!el) return nullptr;
    return (BasePlant*)((el)->base);
}

BaseAnimal * get_base_animal(int32_t id)
{
    BaseListElement *el=(BaseListElement*)base_animals.find(&id);
    if (!el) return nullptr;
    return (BaseAnimal*)((el)->base);
}

ElementsList * players, * packets_to_send;
int players_id;

ENetHost * server;

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

void send_to_all(Packet *p)
{
    ListElement * pl_el = players->head;
    while (pl_el)
    {
        PlayerClient * pl = (PlayerClient*) pl_el;
        printf("sending to player: %d\n", pl->player->get_id());
        p->send(pl->peer);
        pl_el = pl_el->next;
    }
}
class PacketToSend : public ListElement
{
    Packet *p;
  public:
    PacketToSend(Packet *p): p(p) {}
    void send(ENetPeer * peer) {
        p->send(peer);
    }
    void to_all()
    {
        send_to_all(p);
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


bool handle_packet(ENetPacket * packet, ENetPeer * peer)
{
    unsigned char * data = packet->data;
    printf("Received length=%lu: %d\n", packet->dataLength, *data);

    Packet * p = check_packet('R', data, packet->dataLength);
    if (!p)
        return false;

    Peer_id peer_id;
    peer_id.tag = Peer_id::Tag::Peer;
    peer_id.peer = peer;
    PlayerClient *pl=nullptr;
    if (players->nr_elements)
        pl = (PlayerClient*) players->find(&peer_id);

    switch (p->get_type())
    {
        case PACKET_JOIN_REQUEST:
        {
            delete p;
            p = new PacketPlayerId(players_id++);
            p->send(peer);
            PlayerClient *new_pl = new PlayerClient(new PlayerServer(((PacketPlayerId*)p)->get_id()), peer);
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
            PacketPlayerMove * move = dynamic_cast<PacketPlayerMove *>(p);
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
    return true;
}

void send_updates()
{
    if (!players->nr_elements) return;

    if (packets_to_send->nr_elements)
    {
        ListElement * el = packets_to_send->head;
        printf("sending updates\n");
        while (el)
        {
            PacketToSend * p = (PacketToSend*) el;
            p->to_all();
            el = el->next;
        }
    }

    if (objects_to_create.nr_elements)
    {
        ListElement * el = objects_to_create.head;
        while (el)
        {
            printf("sending objects to create: %s id=%lx\n", el->el->get_name(), el->el->uid);
            Packet * p = new PacketObjectCreate(el->el);
            send_to_all(p);
            el = el->next;
        }

    }
    packets_to_send->remove_all();
    objects_to_create.remove_all();
}

void load_chunk(int cx, int cy)
{
    printf("load_chunk(%d, %d)\n", cx, cy);
    chunk * ch = new chunk(cx, cy);

    for (int y = 0; y < CHUNK_SIZE; y++)
        for (int x = 0; x < CHUNK_SIZE; x++)
            ch->table[y][x].tile = cx+cy;

    BaseListElement * base_el = (BaseListElement*) base_elements.get_random();
    ch->add_object( create_element((BaseElement*)(base_el->base)));

    base_el = (BaseListElement*) base_plants.get_random();
    ch->add_object( create_plant((BasePlant*)(base_el->base)));

    base_el = (BaseListElement*) base_animals.get_random();
    ch->add_object( create_animal((BaseAnimal*)(base_el->base)));

   // ch->add_object(create_scroll(new Base(rand() % 10, Class_Scroll,"scroll")));

    world_table[cy][cx] = ch;
}

void add_packet_to_send(Packet *p)
{
    if (players->nr_elements)
        packets_to_send->add(new PacketToSend(p));
}

void notify_update(const InventoryElement * el)
{
    add_packet_to_send(new PacketObjectUpdate((InventoryElement*) el));
}

void update_location(size_t id, ItemLocation old_loc, ItemLocation new_loc)
{
   /* printf("update location uid=%lx old_tag=%d new_tag=%d\n", id, (int)old_loc.tag, (int)new_loc.tag);
    old_loc.show();
    new_loc.show();
*/
    add_packet_to_send(new PacketLocationUpdate(id, old_loc, new_loc));
}
void notify_destroy(size_t id, ItemLocation location)
{
    add_packet_to_send(new PacketObjectDestroy(id, location));
}
void notify_knowledge(size_t pl_id, Class_id cid, int id)
{
    add_packet_to_send(new PacketKnowledgeUpdate(pl_id, cid, id));
}
void notify_checked(size_t pl_id, size_t el)
{
    add_packet_to_send(new PacketCheckedUpdate(pl_id, el));
}

void generate()
{
    players = new InvList("Players");
    for (int i=0; i < 5; i++)
    {
        enum Form form;
        int f = rand() % 10;
        if (f < 8) form = Form_solid;
        else form = Form_liquid;
        ListElement * entry = new BaseListElement(new BaseElement(form, i));
        base_elements.add(entry);
        entry = new BaseListElement(new BasePlant(i));
        base_plants.add(entry);
        entry = new BaseListElement(new BaseAnimal(i));
        base_animals.add(entry);
    }
}

int main()
{
    trace_network = 1;

    if (enet_initialize() != 0)
    {
        fprintf(stderr, "Can't initialize ENet\n");
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
        fprintf(stderr, "Can't create host\n");
        return 1;
    }
    printf("Server Pime started on port %u\n", address.port);

    generate();

    for (int cy=127; cy < 130; cy++)
        for (int cx=127; cx < 130; cx++)
            load_chunk(cy, cx);

    packets_to_send = new ElementsList("packets to send");

    ENetEvent event;
    char hostname[512] = {
        0,
    };

    while (enet_host_service(server, &event, 20) >= 0)
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
                break;
            }
            case ENET_EVENT_TYPE_RECEIVE:
            {
                handle_packet(event.packet, event.peer);
                enet_packet_destroy(event.packet);
                send_updates();
                break;
            }
            case ENET_EVENT_TYPE_DISCONNECT:
            {
                Peer_id peer_id;
                peer_id.tag = Peer_id::Tag::Peer;
                peer_id.peer = event.peer;
                PlayerClient *pl = (PlayerClient*) players->find(&peer_id);
                enet_address_get_host_ip(&event.peer->address, hostname, 512);

                printf("player: %s from %s:%d disconnected\n", pl->player->get_name(), hostname, event.peer->address.port);
                destroy(pl->player);
                players->remove(pl);
                send_updates();
                event.peer->data = NULL;
                break;
            }
            default:
                //printf("time=%ld\n", get_time_ms());
                update();
                send_updates();
                break;
        }
    }

    enet_host_destroy(server);
    return 0;
}

