#include <stdint.h>
#include "../../core/alchemist/elements.h"
#include "../../core/tiles.h"
#include "../../core/packet_types.h"
#include "../../SDL/networking.h"
#include <enet/enet.h>
#include <cstring>
#include "../../server/enet/server.h"

bool handle_packet(ENetPacket * packet)
{
    unsigned char * data=packet->data;
    printf("Received length=%lu: %d\n", packet->dataLength, *data);
    show_packet_type_name('C', *data);
    
    switch(*data)
    {
        case PACKET_PLAYER_ID:
            if (packet->dataLength == sizeof(struct Player_ID))
            {
                got_id(data[1], 0);
                return true;
            }
    }
    return false;
}

NetClient *init(const char *server_ip, const char *port) 
{
    trace_network = 1;

    if (enet_initialize() != 0) {
        fprintf(stderr, "Enet initialization failed\n");
        exit(EXIT_FAILURE);
    }
    atexit(enet_deinitialize);

    ENetHost *client = enet_host_create(NULL, 1, 2, 0, 0);
    if (!client) {
        fprintf(stderr, "Can't create client\n");
        exit(EXIT_FAILURE);
    }

    ENetAddress address;
    enet_address_set_host(&address, server_ip);
    address.port = atoi(port);

    ENetPeer *peer = enet_host_connect(client, &address, 2, 0);
    if (!peer) {
        fprintf(stderr, "Can't connect to server\n");
        exit(EXIT_FAILURE);
    }

    ENetEvent event;

    if (enet_host_service(client, &event, 5000) > 0   // 5000 ms
        && event.type == ENET_EVENT_TYPE_CONNECT) {
        
        char hostname[256];
        enet_address_get_host_ip(&event.peer->address, hostname, 256);
        printf("Connected to %s:%d\n", hostname, event.peer->address.port);

    } else {
        enet_peer_reset(peer);
        printf("Can't connect to server\n");
        exit(EXIT_FAILURE);
    }

    PacketType p = PACKET_JOIN_REQUEST;
    ENetPacket *packet = enet_packet_create(&p, 1, ENET_PACKET_FLAG_RELIABLE);
    enet_peer_send(peer, 0, packet);
    enet_host_flush(client);

    if (enet_host_service(client, &event, 5000) > 0
        && event.type == ENET_EVENT_TYPE_RECEIVE) 
    {
        handle_packet(event.packet);
        enet_packet_destroy(event.packet);
        return (NetClient*) client;
    } 
    else 
    {
        printf("Server did not answer\n");
    
        enet_peer_disconnect(peer, 0);

        while (enet_host_service(client, &event, 3000) > 0) {
            if (event.type == ENET_EVENT_TYPE_DISCONNECT) {
                printf("Disconnected from server\n");
                break;
            }
        }
        exit(0);
    }

  }

uint32_t network_tick(NetClient *client) { return 0; }

BaseElement *get_base_element(int32_t id) {return nullptr; }

BasePlant *get_base_plant(int32_t id){return nullptr; }

BaseAnimal *get_base_animal(int32_t id){return nullptr; }

Base *get_base(uint32_t c_id, int32_t id){return nullptr; }

void send_packet_move(NetClient *client, int32_t x, int32_t y){ 
     //player_server->move(x, y);
}

void send_packet_pickup(NetClient *client, uintptr_t id){ }

void send_packet_drop(NetClient *client, uintptr_t id){ }

void send_packet_item_used_on_object(NetClient *client, uintptr_t iid, uintptr_t oid){ }

void send_packet_action_on_object(NetClient *client, int32_t a, uintptr_t oid){ }

void send_packet_server_action_on_object(NetClient *client, int32_t a, uintptr_t oid){ }

void send_packet_item_used_on_tile(NetClient *client,
                                   uintptr_t iid,
                                   int32_t map_x,
                                   int32_t map_y,
                                   int32_t x,
                                   int32_t y){ }

void send_packet_craft(NetClient *client,
                       uintptr_t prod_id,
                       uintptr_t ingredients_num,
                       const uintptr_t *iid){ }

void send_packet_request_chunk(NetClient *client, int32_t cx, int32_t cy)
{ 
    chunk * ch = new chunk(cx, cy);

    for (int y = 0; y < CHUNK_SIZE; y++)
        for (int x = 0; x < CHUNK_SIZE; x++)
            ch->table[y][x].tile = 1;
    
    update_chunk(cx, cy, (chunk_table*) (((uint8_t *)ch->table) - 3)); //rust adds packet type, x, y as 3 bytes
}



