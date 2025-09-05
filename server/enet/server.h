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

  protected:
  int send_data(ENetPeer * peer, void * data, size_t size)
  {
      show_packet_type_name('S', *(unsigned char*)data);
      ENetPacket * p = enet_packet_create(data, size, ENET_PACKET_FLAG_RELIABLE);
      int ret = enet_peer_send(peer, 0, p);
      enet_host_flush(peer->host);
      return ret;
  }
  public:
    Packet(PacketType t) : t(t)
    {
    }
    virtual ~Packet()
    {
    }
    virtual int send(ENetPeer * peer)
    {
        return send_data(peer, &t, sizeof(PacketType));
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
        return check_size(s);
    }
};

class PacketJoinRequest : public Packet
{
  public:
    PacketJoinRequest() : Packet(PACKET_JOIN_REQUEST)
    {
    }    
};

class PacketPlayerId : public Packet
{
    struct serial_data
    {
        PacketType t;
        unsigned long id;
    } data, *pdata __attribute__((packed));

  public:
    PacketPlayerId(unsigned long id) : Packet(PACKET_PLAYER_ID)
    {      
        data.t = t;
        data.id=id;
    }
    PacketPlayerId() : Packet(PACKET_PLAYER_ID)
    {

    }
    bool update(unsigned char * net_data, size_t s)
    {
        if (check_size(s))
        {
            pdata = (struct serial_data *)net_data;
            return true;
        }
        return false;
    }
    int send(ENetPeer * peer)
    {
        return send_data(peer, &data, sizeof(struct serial_data));
    }
    bool check_size(int s)
    {
        return s == sizeof(struct serial_data);
    }
    unsigned long get_id()
    {
        return pdata->id;
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
        case Class_Player:
        {
            Player * player = dynamic_cast<Player *>(el);
            obj = new (0) ObjectData(ObjectData::Tag::Player, sizeof(ObjectData));
            obj->player.data = *player;
            obj->player.data.clan = nullptr;
            obj->player.data.player_skills = nullptr;
            obj->player.data.inventory = nullptr;
            obj->player.data.known_elements = nullptr;
            obj->player.data.talking_to = nullptr;
            obj->player.data.relations = nullptr;
            return obj;
        }
        default:
            printf("Unknown class ID in convert_to_data\n");
            break;
    }
    return obj;
}

class PacketObjectCreate : public Packet
{
    struct serial_data
    {
        PacketType t;
        size_t size;
        unsigned char data[0];
        static void * operator new(size_t size_base, size_t extra)
        {
            printf("serial_data: allocating %ld + %ld\n", size_base, extra);
            return ::operator new(size_base + extra);
        }
        serial_data(size_t s) : size(s)
        {
        }
        static void operator delete(void * ptr)
        {
            ::operator delete(ptr);
        }
    } __attribute__((packed));

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
        int ret=0;
        struct serial_data * d = new (obj->size) serial_data(sizeof(serial_data) + obj->size);
        d->t = t;
        memcpy(d->data, (void *)obj, obj->size);

        /*for (int i=0; i< 100; i++)
            printf("[%d] = %d %x\n", i, obj->data[i], (obj->data[i]));
*/
        ret = send_data(peer, d, d->size);
        delete(d);
        return ret;
    }
    bool update(unsigned char * data, size_t s)
    {
        struct serial_data * d = (struct serial_data *)data;
        if (s != d->size)
            return false;
        obj = (ObjectData *)(&d->data);
/*      for (int i=0; i<100; i++)
            printf("[%d] = %d %x\n", i, obj->data[i], (obj->data[i]));
*/
        printf("PacketObjectCreate for objectData::Tag=%d\n", (int)obj->tag);
        switch (obj->tag)
        {
            case ObjectData::Tag::Element:
            {
                BaseElement * b = (BaseElement *)&obj->data;
                obj->element.data.set_base(new BaseElement(*b));
                break;
            }
            case ObjectData::Tag::Player:
                break;
            default:                
                break;
        }

        return true;
    }
};

class PacketChunkUpdate : public Packet
{
    struct serial_data
    {
        PacketType t;
        unsigned char x, y;
        chunk_table table;
    } data, *pdata __attribute__((packed));
    chunk_table *ptable;

  public:
    unsigned char get_x() { return pdata->x; }
    unsigned char get_y() { return pdata->y; }
    chunk_table * get_table() { return ptable; }
    PacketChunkUpdate(unsigned char x, unsigned char y) : Packet(PACKET_CHUNK_UPDATE)
    {
        data.t = t;
        data.x = x;
        data.y = y;
        memcpy(&data.table, &world_table[y][x]->table , sizeof(chunk_table));
    }
    PacketChunkUpdate() : Packet(PACKET_CHUNK_UPDATE)
    {
    }
    int send(ENetPeer * peer)
    {
        int ret;       
        ret = send_data(peer, &data, sizeof(struct serial_data));

        chunk * ch = world_table[data.y][data.x];
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
    bool update(unsigned char * net_data, size_t s)
    {
        if (check_size(s))
        {
            pdata = (struct serial_data *)net_data;
            // rust version adds packet type, x, y as 3 bytes
            ptable = (chunk_table *)(((unsigned char *)&pdata->table) - 3);
            return true;
        }
        return false;
    }
    bool check_size(int s)
    {
        return s == sizeof(struct serial_data);
    }
};

class PacketPlayerMove : public Packet
{
    struct serial_data
    {
        PacketType t;
        int x, y;
    } data, *pdata __attribute__((packed));

  public:
    PacketPlayerMove(int x, int y) : Packet(PACKET_PLAYER_MOVE)
    {
        data.t = t;
        data.x = x;
        data.y = y;
    }
    PacketPlayerMove() : Packet(PACKET_PLAYER_MOVE)
    {
    }
    bool update(unsigned char * net_data, size_t s)
    {
        if (check_size(s))
        {
            pdata = (struct serial_data *)net_data;
            return true;
        }
        return false;
    }
    int send(ENetPeer * peer)
    {
        return send_data(peer, &data, sizeof(struct serial_data));
    }
    bool check_size(int s)
    {
        return s == sizeof(struct serial_data);
    }
    unsigned int get_x()
    {
        return pdata->x;
    }
    unsigned int get_y()
    {
        return pdata->y;
    }
};

class PacketLocationUpdate : public Packet
{
    struct serial_data
    {
        PacketType t;
        LocationUpdateData location;
    } data, *pdata __attribute__((packed));

  public:
    LocationUpdateData get_location() { return pdata->location ; }

    PacketLocationUpdate(size_t i, ItemLocation old_loc, ItemLocation new_loc) : Packet(PACKET_LOCATION_UPDATE)
    {
        data.t = t;
        data.location.id = i;
        data.location.old = old_loc;
        data.location.new_ = new_loc;
    }
    PacketLocationUpdate() : Packet(PACKET_LOCATION_UPDATE)
    {}
    bool update(unsigned char * net_data, size_t s)
    {
        if (check_size(s))
        {
            pdata = (struct serial_data *)net_data;
            return true;
        }
        return false;
    }
    int send(ENetPeer * peer)
    {
        return send_data(peer, &data, sizeof(struct serial_data));
    }
    bool check_size(int s)
    {
        return s == sizeof(struct serial_data);
    }
};

class PacketRequestChunk : public Packet
{
    struct serial_data
    {
        PacketType t;
        int cx, cy;
    } data, *pdata __attribute__((packed));

  public:
    int get_cx() { return pdata->cx; }
    int get_cy() { return pdata->cy; }

    PacketRequestChunk(int cx, int cy) : Packet(PACKET_REQUEST_CHUNK)
    {
        data.t = t;
        data.cx = cx;
        data.cy = cy;
    }
    PacketRequestChunk() : Packet(PACKET_REQUEST_CHUNK)
    {}
    bool update(unsigned char * net_data, size_t s)
    {
        if (check_size(s))
        {
            pdata = (struct serial_data *)net_data;
            return true;
        }
        return false;
    }
    int send(ENetPeer * peer)
    {
        return send_data(peer, &data, sizeof(struct serial_data));
    }
    bool check_size(int s)
    {
        return s == sizeof(struct serial_data);
    }
};

Packet * check_packet(char dir, unsigned char * data, size_t s)
{    
    Packet * p = nullptr;
    switch (data[0])
    {
        case PACKET_JOIN_REQUEST:    p = new PacketJoinRequest();    break;
        case PACKET_PLAYER_ID:       p = new PacketPlayerId();       break;
        case PACKET_PLAYER_MOVE:     p = new PacketPlayerMove();     break;
        case PACKET_CHUNK_UPDATE:    p = new PacketChunkUpdate();    break;
        case PACKET_OBJECT_CREATE:   p = new PacketObjectCreate();   break;
        case PACKET_LOCATION_UPDATE: p = new PacketLocationUpdate(); break;
        case PACKET_REQUEST_CHUNK:   p = new PacketRequestChunk(); break;
    }
    if (p->update(data, s))
    {
        show_packet_type_name(dir, p->get_type());
        return p;
    }
    else
    {
        delete p;
        printf("UNKNOWN PACKET: %d with size: %ld\n", data[0], s);
        return nullptr;
    }
}
