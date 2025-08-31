#include <stdint.h>
#include "../../core/alchemist/elements.h"
#include "../../core/tiles.h"
#include <enet/enet.h>
#include "../../SDL/networking.h"
#include <cstring>
#include "../../server/enet/server.h"

bool handle_packet(ENetPacket * packet)
{
    bool ret = false;
    unsigned char * data = packet->data;
    printf("Received length=%lu: %d\n", packet->dataLength, *data);

    Packet * p = check_packet('C', data, packet->dataLength);
    if (!p)
        return ret;

    switch (p->get_type())
    {
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
            update_chunk(up->x, up->y, up->table);
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
    }
    delete p;
    return ret;
}

NetClient * init(const char * server_ip, const char * port)
{
    trace_network = 1;

    if (enet_initialize() != 0)
    {
        fprintf(stderr, "Enet initialization failed\n");
        exit(EXIT_FAILURE);
    }
    atexit(enet_deinitialize);

    client = enet_host_create(NULL, 1, 2, 0, 0);
    if (!client)
    {
        fprintf(stderr, "Can't create client\n");
        exit(EXIT_FAILURE);
    }

    ENetAddress address;
    enet_address_set_host(&address, server_ip);
    address.port = atoi(port);

    ENetPeer * peer = enet_host_connect(client, &address, 2, 0);
    if (!peer)
    {
        fprintf(stderr, "Can't connect to server\n");
        exit(EXIT_FAILURE);
    }

    ENetEvent event;

    if (enet_host_service(client, &event, 5000) > 0 // 5000 ms
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
        exit(EXIT_FAILURE);
    }

    Packet * p = new PacketJoinRequest();
    p->send(peer);
    enet_host_flush(client);

    if (enet_host_service(client, &event, 5000) > 0 && event.type == ENET_EVENT_TYPE_RECEIVE)
    {
        handle_packet(event.packet);
        enet_packet_destroy(event.packet);
        return (NetClient *)client;
    }
    else
    {
        printf("Server did not answer\n");

        enet_peer_disconnect(peer, 0);

        while (enet_host_service(client, &event, 3000) > 0)
        {
            if (event.type == ENET_EVENT_TYPE_DISCONNECT)
            {
                printf("Disconnected from server\n");
                break;
            }
        }
        exit(0);
    }
}

unsigned int network_tick(NetClient * client)
{
    ENetEvent event;
    unsigned int recv = 0;
    while (enet_host_service(client, &event, 10) > 0)
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
InventoryElement * get_object_by_id(uintptr_t uid)
{
    return nullptr;
}

void register_object(InventoryElement * o)
{
}

void deregister_object(InventoryElement * o)
{
}

BaseElement * get_base_element(int32_t id)
{
    return nullptr;
}

BasePlant * get_base_plant(int32_t id)
{
    return nullptr;
}

BaseAnimal * get_base_animal(int32_t id)
{
    return nullptr;
}

Base * get_base(uint32_t c_id, int32_t id)
{
    return nullptr;
}

void send_packet_move(NetClient * client, int32_t x, int32_t y)
{
}

void send_packet_pickup(NetClient * client, uintptr_t id)
{
}

void send_packet_drop(NetClient * client, uintptr_t id)
{
}

void send_packet_item_used_on_object(NetClient * client, uintptr_t iid, uintptr_t oid)
{
}

void send_packet_action_on_object(NetClient * client, int32_t a, uintptr_t oid)
{
}

void send_packet_server_action_on_object(NetClient * client, int32_t a, uintptr_t oid)
{
}

void send_packet_item_used_on_tile(NetClient * client, uintptr_t iid, ItemLocation location)
{
}

void send_packet_craft(NetClient * client, uintptr_t prod_id, uintptr_t ingredients_num, const uintptr_t * iid)
{
}

void send_packet_request_chunk(NetClient * client, int32_t cx, int32_t cy)
{
}
