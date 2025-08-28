#include <enet/enet.h>
#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include "server.h"

unsigned int players;
ENetHost *server;

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

bool handle_packet(ENetPacket * packet, ENetPeer *peer) 
{
    unsigned char * data=packet->data;
    printf("Received length=%lu: %d\n", packet->dataLength, *data);
    show_packet_type_name('S', *data);

    switch(*data)
    {
        case PACKET_JOIN_REQUEST:
            if (packet->dataLength == 1)
            {
                struct Player_ID p = { PACKET_PLAYER_ID, players++ };
                ENetPacket *response = enet_packet_create(&p, sizeof(struct Player_ID), ENET_PACKET_FLAG_RELIABLE);
                enet_peer_send(peer, 0, response);
                enet_host_flush(server);
            }
    }
    return false;
}

int main() {
    trace_network = 1;

    if (enet_initialize() != 0) {
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
    if (!server) {
        fprintf(stderr, "Nie udało się utworzyć hosta serwera\n");
        return 1;
    }
    printf("Server Pime started on port %u\n", address.port);

    ENetEvent event;
    char hostname[512] = { 0, };

    while (enet_host_service(server, &event, 1000) >= 0) {
        switch (event.type) {
            case ENET_EVENT_TYPE_CONNECT:
                {
                    enet_address_get_host_ip(&event.peer->address, hostname, 512);
                    printf("Client connected %s:%d\n", hostname, event.peer->address.port);
                    unsigned short *port = new unsigned short;
                    *port = event.peer->address.port;
                    event.peer->data = (void*) port; 
                }
                break;

            case ENET_EVENT_TYPE_RECEIVE:
                {
                    handle_packet(event.packet, event.peer);
                    enet_packet_destroy(event.packet);
                }
                break;

            case ENET_EVENT_TYPE_DISCONNECT:
                printf("Client [%d] disconnected ", *(unsigned short*)(event.peer->data));
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
