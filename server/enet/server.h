#include <enet/enet.h>
#include <stdio.h>
#include <string.h>
#include "../../core/packet_types.h"
#include "../../core/tiles.h"
#include "../../core/world.h"
#include "../../SDL/networking.h"

struct TerrainType
{
    unsigned int id;
    BaseElement base;
};
struct PlantType
{
    unsigned int id;
    BasePlant base;
};

struct AnimalType
{
    unsigned int id;
    BaseAnimal base;
};

class Packet
{
  protected:
    PacketType t;

  public:
    Packet(PacketType t) : t(t)
    {
    }
    virtual ~Packet()
    {
    }
    virtual int send(ENetPeer * peer)
    {
        ENetPacket * p = enet_packet_create(&t, sizeof(PacketType), ENET_PACKET_FLAG_RELIABLE);
        return enet_peer_send(peer, 0, p);
    }
    PacketType get_type()
    {
        return t;
    }
    virtual bool check_size(int s)
    {
        return s == sizeof(PacketType);
    }
    virtual bool update(unsigned char * data, size_t s)
    {
        return false;
    }
};

class PacketJoinRequest : public Packet
{
  public:
    PacketJoinRequest() : Packet(PACKET_JOIN_REQUEST)
    {
    }
};

struct Player_ID
{
    PacketType t;
    unsigned int id;
} __attribute__((packed));

class PacketPlayerId : public Packet
{
    unsigned int id;

  public:
    PacketPlayerId(unsigned int id) : Packet(PACKET_PLAYER_ID), id(id)
    {
    }
    PacketPlayerId() : Packet(PACKET_PLAYER_ID), id(0)
    {
    }
    bool update(unsigned char * data, size_t s)
    {
        if (check_size(s))
        {
            struct Player_ID * d = (struct Player_ID *)data;
            id = d->id;
            return true;
        }
        return false;
    }
    int send(ENetPeer * peer)
    {
        struct Player_ID d = {t, id};
        ENetPacket * p = enet_packet_create(&d, sizeof(struct Player_ID), ENET_PACKET_FLAG_RELIABLE);
        return enet_peer_send(peer, 0, p);
    }
    bool check_size(int s)
    {
        return s == sizeof(struct Player_ID);
    }
    unsigned int get_id()
    {
        return id;
    }
};

ObjectData * convert_to_data(InventoryElement * el)
{
    ObjectData * obj = nullptr;
    switch (el->c_id)
    {
        case Class_Element:
        {
            Element * element = dynamic_cast<Element *>(el);
            BaseElement * base = element->get_base();
            obj = new (sizeof(*base)) ObjectData(ObjectData::Tag::Element, sizeof(ObjectData) + sizeof(*base));
            obj->element.data = *element;
            BaseElement * b = (BaseElement *)&obj->data;
            *b = *base;
            return obj;
        }
        default:
            printf("Unknown class ID in convert_to_data\n");
            break;
    }
    return obj;
}

struct Packet_object_create
{
    PacketType t;
    size_t size;
    unsigned char data[0];
    static void * operator new(size_t size_base, size_t extra)
    {
        printf("Packet_object_create: allocating %ld + %ld\n", size_base, extra);
        return ::operator new(size_base + extra);
    }
    Packet_object_create(size_t s) : size(s)
    {
    }
    static void operator delete(void * ptr)
    {
        ::operator delete(ptr);
    }
} __attribute__((packed));

class PacketObjectCreate : public Packet
{
  public:
    ObjectData * obj;
    PacketObjectCreate(InventoryElement * el) : Packet(PACKET_OBJECT_CREATE)
    { // called by server
        obj = convert_to_data(el);
    }
    PacketObjectCreate() : Packet(PACKET_OBJECT_CREATE)
    { // called by client
        obj = nullptr;
    }
    int send(ENetPeer * peer)
    {
        struct Packet_object_create * d = new (obj->size) Packet_object_create(sizeof(Packet_object_create) + obj->size);
        d->t = t;
        memcpy(d->data, (void *)obj, obj->size);
        show_packet_type_name('S', t);
        /*for (int i=0; i< 100; i++)
            printf("[%d] = %d %x\n", i, obj->data[i], (obj->data[i]));
*/
        ENetPacket * p = enet_packet_create(d, d->size, ENET_PACKET_FLAG_RELIABLE);
        return enet_peer_send(peer, 0, p);
    }
    bool update(unsigned char * data, size_t s)
    {
        struct Packet_object_create * d = (struct Packet_object_create *)data;
        if (s != d->size)
            return false;
        obj = (ObjectData *)(&d->data);

        BaseElement * b = (BaseElement *)&obj->data;
        /*    for (int i=0; i<100; i++)
                printf("[%d] = %d %x\n", i, obj->data[i], (obj->data[i]));
*/
        obj->element.data.set_base(new BaseElement(*b));

        return true;
    }
};

struct Packet_chunk_update
{
    PacketType t;
    unsigned char x, y;
    chunk_table table;
} __attribute__((packed));

class PacketChunkUpdate : public Packet
{
  public:
    unsigned char x, y;
    chunk_table * table;
    PacketChunkUpdate(unsigned char x, unsigned char y) : Packet(PACKET_CHUNK_UPDATE), x(x), y(y)
    {
        table = &world_table[y][x]->table;
    }
    PacketChunkUpdate() : Packet(PACKET_CHUNK_UPDATE)
    {
        table = nullptr;
        x = 0;
        y = 0;
    }
    int send(ENetPeer * peer)
    {
        int ret;
        struct Packet_chunk_update d1;
        d1.t = t;
        d1.x = x;
        d1.y = y;
        show_packet_type_name('T', t);
        memcpy(&d1.table, table, sizeof(chunk_table));
        ENetPacket * p1 = enet_packet_create(&d1, sizeof(struct Packet_chunk_update), ENET_PACKET_FLAG_RELIABLE);
        ret = enet_peer_send(peer, 0, p1);

        chunk * ch = world_table[y][x];
        ch->objects.show(false);
        ch->beings.show(false);
        ListElement * el = ch->objects.head;
        while (el)
        {
            Packet * p = new PacketObjectCreate(el->el);
            ret = p->send(peer);
            el = el->next;
        }
        return ret;
    }
    bool update(unsigned char * data, size_t s)
    {
        if (check_size(s))
        {
            struct Packet_chunk_update * d = (struct Packet_chunk_update *)data;
            x = d->x;
            y = d->y;
            // rust version adds packet type, x, y as 3 bytes
            table = (chunk_table *)(((unsigned char *)&d->table) - 3);
            return true;
        }
        return false;
    }
    bool check_size(int s)
    {
        return s == sizeof(struct Packet_chunk_update);
    }
};

Packet * check_packet(char who, unsigned char * data, size_t s)
{
    bool check = false;
    Packet * p = nullptr;
    switch (data[0])
    {
        case PACKET_JOIN_REQUEST:
        {
            p = new PacketJoinRequest();
            if (p->check_size(s))
                check = true;
            else
                delete p;
            break;
        }
        case PACKET_PLAYER_ID:
        {
            p = new PacketPlayerId();
            if (p->update(data, s))
                check = true;
            else
                delete p;
            break;
        }
        case PACKET_CHUNK_UPDATE:
        {
            p = new PacketChunkUpdate();
            if (p->update(data, s))
                check = true;
            else
                delete p;
            break;
        }
        case PACKET_OBJECT_CREATE:
        {
            p = new PacketObjectCreate();
            if (p->update(data, s))
                check = true;
            else
                delete p;
            break;
        }
    }
    if (check)
    {
        show_packet_type_name(who, p->get_type());
        return p;
    }
    printf("UNKNOWN PACKET: %d with size: %ld\n", data[0], s);
    return nullptr;
}
