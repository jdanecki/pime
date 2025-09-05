#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include "server.h"
#include "../../core/alchemist/el_list.h"
#include "../cpp-src/player_server.h"
#include "../cpp-src/elements_server.h"

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
Packet *packet_to_send;

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

ElementsList * players;
ENetHost * server;

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

    switch (p->get_type())
    {
        case PACKET_JOIN_REQUEST:
        {
            delete p;
            p = new PacketPlayerId(players->nr_elements);
            p->send(peer);            
            PlayerClient *pl = new PlayerClient(new PlayerServer(players->nr_elements), peer);
            players->add(pl);
            add_object_to_world(pl->player, pl->player->location);
            delete p;
            p = new PacketChunkUpdate(128, 128);
            p->send(peer);            
            delete p;
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
            if (packet_to_send)
            {
                packet_to_send->send(peer);
                packet_to_send = nullptr;
            }
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
    }
    return false;
}
extern "C"
{

    void load_chunk(int cx, int cy)
    {
        printf("load_chunk(%d, %d)\n", cx, cy);
        chunk * ch = new chunk(cx, cy);

        for (int y = 0; y < CHUNK_SIZE; y++)
            for (int x = 0; x < CHUNK_SIZE; x++)
                ch->table[y][x].tile = cx + cy;

        ElementServer * el = create_element(new BaseElement(Form_solid, cx + cy));
      //  el->show(true);
        ch->add_object(el);
        world_table[cy][cx] = ch;
    }

    void notify_update(const InventoryElement * el)
    {
    }

    void update_location(size_t id, ItemLocation old_loc, ItemLocation new_loc)
    {
        printf("update location id=%ld old_tag=%d new_tag=%d\n", id, (int)old_loc.tag, (int)new_loc.tag);
        old_loc.show();
        new_loc.show();

        packet_to_send = new PacketLocationUpdate(id, old_loc, new_loc);
    }
    void notify_destroy(size_t id, ItemLocation location)
    {
    }
    void notify_knowledge(size_t pl_id, Class_id cid, int id)
    {
    }
    void notify_checked(size_t pl_id, size_t el)
    {
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
