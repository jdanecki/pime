#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include "server.h"
#include "../../core/alchemist/el_list.h"
#include "../cpp-src/player_server.h"
#include "../cpp-src/elements_server.h"

InvList * players;
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
};

bool handle_packet(ENetPacket * packet, ENetPeer * peer)
{
    unsigned char * data = packet->data;
    printf("Received length=%lu: %d\n", packet->dataLength, *data);

    Packet * p = check_packet('S', data, packet->dataLength);
    if (!p)
        return false;

    switch (p->get_type())
    {
        case PACKET_JOIN_REQUEST:
        {
            Packet * p = new PacketPlayerId(players->nr_elements);
            p->send(peer);
            enet_host_flush(server);
            players->add(new PlayerServer(players->nr_elements));
            delete p;
            p = new PacketChunkUpdate(128, 128);
            p->send(peer);
            enet_host_flush(server);
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
        chunk * ch = new chunk(cx, cy);

        for (int y = 0; y < CHUNK_SIZE; y++)
            for (int x = 0; x < CHUNK_SIZE; x++)
                ch->table[y][x].tile = 1;
        ElementServer * el = create_element(new BaseElement(Form_solid, 1));
        el->show(true);
        ch->add_object(el);
        world_table[cy][cx] = ch;
    }

    void update_location(size_t id, ItemLocation old_loc, ItemLocation new_loc)
    {
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
    load_chunk(128, 128);

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
