#include <stdint.h>
#include "../../core/alchemist/elements.h"
#include "../../core/tiles.h"
#include <enet/enet.h>
#include "../../SDL/networking.h"
#include <cstring>
#include "../../server/enet/server.h"

ElementsList base_elements("base elements");
ElementsList base_plants("base plants");
ElementsList base_animals("base animals");

ElementsList objects("objects");

class ObjectElement : public ListElement
{
  public:
    ObjectElement(InventoryElement *el): ListElement(el) {}
    bool check(void *what)
    {
        uintptr_t * uid=(uintptr_t*)what;
        return *uid == el->uid;
    }
};

bool handle_packet(ENetPacket * packet)
{
    bool ret = false;
    unsigned char * data = packet->data;
    printf("Received length=%lu: %d\n", packet->dataLength, *data);

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

            destroy_object(obj->get_id(), obj->get_location());
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
InventoryElement * get_object_by_id(uintptr_t uid)
{
    ListElement *el = objects.find(&uid);
    return el ? el->el : nullptr;
}

void register_object(InventoryElement * o)
{
    ObjectElement *obj = new ObjectElement(o);
    printf("register_object: uid=%lx\n", o->uid);
    objects.add(obj);
}

void deregister_object(InventoryElement * o)
{
    ListElement * obj= objects.find(&o->uid);
    objects.remove(obj);
}

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
