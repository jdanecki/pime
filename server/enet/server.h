#include <enet/enet.h>
#include <stdio.h>
#include <string.h>
#include "../../core/packet_types.h"
#include "../../core/tiles.h"
#include "../../core/world.h"
#include "../../SDL/networking.h"
#include <assert.h>

extern ElementsList base_elements;
extern ElementsList base_plants;
extern ElementsList base_animals;

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

class PacketActionFailed : public Packet
{
  public:
    PacketActionFailed() : Packet(PACKET_ACTION_FAILED)
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

class PacketObjectDestroy : public Packet
{
    struct serial_data
    {
        PacketType t;
        unsigned long id;
        ItemLocation location;
    } data, *pdata __attribute__((packed));

  public:
    unsigned long get_id()
    {
        return pdata->id;
    }
    ItemLocation get_location()
    {
        return pdata->location;
    }
    PacketObjectDestroy(unsigned long id, ItemLocation location) : Packet(PACKET_OBJECT_DESTROY)
    {
        data.t = t;
        data.id=id;
        data.location=location;
    }
    PacketObjectDestroy() : Packet(PACKET_OBJECT_DESTROY)
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
        //FIXME send as broadcast
        return send_data(peer, &data, sizeof(struct serial_data));
    }
    bool check_size(int s)
    {
        return s == sizeof(struct serial_data);
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
            break;
        }
        case Class_Player:
        {
            Player * player = dynamic_cast<Player *>(el);
            obj = new ObjectData(ObjectData::Tag::Player);
            obj->player.data = *player;
            obj->player.data.clan = nullptr;
            obj->player.data.player_skills = nullptr;
            obj->player.data.inventory = nullptr;
            obj->player.data.known_elements = nullptr;
            obj->player.data.talking_to = nullptr;
            obj->player.data.relations = nullptr;
            break;

        }
        case Class_Ingredient:
        {
            Ingredient * ing = dynamic_cast<Ingredient *>(el);
            obj = new ObjectData(ObjectData::Tag::Ingredient);
            obj->ingredient.data = *ing;
            break;
        }
        case Class_Product:
        {
            Product * prod = dynamic_cast<Product *>(el);
            obj = new ObjectData(ObjectData::Tag::Product);
            obj->product.data = *prod;
            break;
        }
        case Class_Plant:
        {
            Plant * plant = dynamic_cast<Plant *>(el);
            obj = new ObjectData(ObjectData::Tag::Plant);
            obj->plant.data = *plant;
            break;
        }
        case Class_Animal:
        {
            Animal * animal = dynamic_cast<Animal *>(el);
            obj = new ObjectData(ObjectData::Tag::Animal);
            obj->animal.data = *animal;
            break;
        }
        case Class_Scroll:
        {
            Scroll * scroll = dynamic_cast<Scroll *>(el);
            obj = new ObjectData(ObjectData::Tag::Scroll);
            obj->scroll.data = *scroll;
            break;
        }
        case Class_Npc:
        {
            Npc * npc = dynamic_cast<Npc *>(el);
            obj = new ObjectData(ObjectData::Tag::Npc);
            obj->npc.data = *npc;
            break;
        }
        default:
            printf("Unknown class ID=%d in convert_to_data\n", el->c_id);
            assert(0);
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
                obj->player.data.inventory = new InvList("inventory");
                obj->player.data.known_elements = new ElementsList("known elements");
                obj->player.data.player_skills = new Skills();
                break;
            default:
                break;
        }
        return true;
    }
};

class PacketObjectUpdate : public Packet
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
    PacketObjectUpdate(InventoryElement * el) : Packet(PACKET_OBJECT_UPDATE)
    { // called by server
        obj = convert_to_data(el);
    }
    PacketObjectUpdate() : Packet(PACKET_OBJECT_UPDATE)
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
        printf("PacketObjectUpdate for objectData::Tag=%d\n", (int)obj->tag);
        switch (obj->tag)
        {
            case ObjectData::Tag::Element:
            {
                BaseElement * b = (BaseElement *)&obj->data;
                obj->element.data.set_base(new BaseElement(*b));
                break;
            }
            case ObjectData::Tag::Player:
                obj->player.data.inventory = new InvList("inventory");
                obj->player.data.known_elements = new ElementsList("known elements");
                obj->player.data.player_skills = new Skills();
                break;
            default:
                break;
        }

        return true;
    }
};

class PacketElementsList : public Packet
{
    struct serial_data
    {
        PacketType t;
        size_t size;
        int nr_elements;
        char name[60];
        Class_id c_id;
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
    } *pdata __attribute__((packed));

  public:
    int get_nr_elements() { return pdata->nr_elements;}
    Class_id get_c_id() {  return pdata->c_id;}
    unsigned char * get_data() {  return pdata->data;}

    PacketElementsList(ElementsList * list) : Packet(PACKET_ELEMENTS_LIST)
    {
        int size = list->nr_elements * list->head->get_size();
        pdata = new (size) serial_data(sizeof(serial_data) + size);
        pdata->t = t;
        pdata->nr_elements = list->nr_elements;
        strncpy(pdata->name, list->name, strlen(list->name)+1);
        pdata->c_id = ((BaseListElement*)list->head)->base->c_id;

        int i=0;
        ListElement * el = list->head;
        while(el)
        {
            BaseListElement * base_el = (BaseListElement*) el;

            switch (pdata->c_id) {
                case Class_BaseElement:
                {
                    BaseElement * dst = &((BaseElement*) &pdata->data)[i];
                    *dst = *((BaseElement*)base_el->base);
                    break;
                }
                case Class_BasePlant:
                {
                    BasePlant * dst = &((BasePlant*) &pdata->data)[i];
                    *dst = *((BasePlant*)base_el->base);
                    break;
                }
                case Class_BaseAnimal:
                {
                    BaseAnimal * dst = &((BaseAnimal*) &pdata->data)[i];
                    *dst = *((BaseAnimal*)base_el->base);
                    break;
                }
            }
            el = el->next;
            i++;
        }
    }
    PacketElementsList() : Packet(PACKET_ELEMENTS_LIST)
    {
    }
    int send(ENetPeer * peer)
    {
        int ret=0;
        /*for (int i=0; i< 100; i++)
            printf("[%d] = %d %x\n", i, obj->data[i], (obj->data[i]));
*/
        ret = send_data(peer, pdata, pdata->size);        
        return ret;
    }
    bool update(unsigned char * data, size_t s)
    {
        pdata = (struct serial_data *)data;
        if (s != pdata->size)
            return false;        
        /*      for (int i=0; i<100; i++)
                    printf("[%d] = %d %x\n", i, obj->data[i], (obj->data[i]));
        */
        printf("PacketElementList %s for %d %s\n", pdata->name, pdata->nr_elements, class_name[pdata->c_id]);
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

class PacketPlayerActionPickup : public Packet
{
    struct serial_data
    {
        PacketType t;
        uintptr_t id;
    } data, *pdata __attribute__((packed));

  public:
    uintptr_t get_id() { return pdata->id; }

    PacketPlayerActionPickup(uintptr_t id) : Packet(PACKET_PLAYER_ACTION_PICKUP)
    {
        data.t = t;
        data.id = id;
    }
    PacketPlayerActionPickup() : Packet(PACKET_PLAYER_ACTION_PICKUP)
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

class PacketPlayerActionDrop : public Packet
{
    struct serial_data
    {
        PacketType t;
        uintptr_t id;
    } data, *pdata __attribute__((packed));

  public:
    uintptr_t get_id() { return pdata->id; }

    PacketPlayerActionDrop(uintptr_t id) : Packet(PACKET_PLAYER_ACTION_DROP)
    {
        data.t = t;
        data.id = id;
    }
    PacketPlayerActionDrop() : Packet(PACKET_PLAYER_ACTION_DROP)
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

class PacketPlayerActionUseItemOnObject : public Packet
{
    struct serial_data
    {
        PacketType t;
        uintptr_t iid;
        uintptr_t oid;
    } data, *pdata __attribute__((packed));

  public:
    uintptr_t get_iid() { return pdata->iid; }
    uintptr_t get_oid() { return pdata->oid; }

    PacketPlayerActionUseItemOnObject(uintptr_t iid, uintptr_t oid) : Packet(PACKET_PLAYER_ACTION_USE_ITEM_ON_OBJECT)
    {
        data.t = t;
        data.iid = iid;
        data.oid = oid;
    }
    PacketPlayerActionUseItemOnObject() : Packet(PACKET_PLAYER_ACTION_USE_ITEM_ON_OBJECT)
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

class PacketPlayerActionOnObject : public Packet
{
    struct serial_data
    {
        PacketType t;
        Player_action a;
        uintptr_t oid;
    } data, *pdata __attribute__((packed));

  public:
    Player_action get_a() { return pdata->a; }
    uintptr_t get_oid() { return pdata->oid; }

    PacketPlayerActionOnObject(Player_action a, uintptr_t oid) : Packet(PACKET_PLAYER_ACTION_ON_OBJECT)
    {
        data.t = t;
        data.a = a;
        data.oid = oid;
    }
    PacketPlayerActionOnObject() : Packet(PACKET_PLAYER_ACTION_ON_OBJECT)
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

class PacketServerActionOnObject : public Packet
{
    struct serial_data
    {
        PacketType t;
        Server_action a;
        uintptr_t oid;
    } data, *pdata __attribute__((packed));

  public:
    Server_action get_a() { return pdata->a; }
    uintptr_t get_oid() { return pdata->oid; }

    PacketServerActionOnObject(Server_action a, uintptr_t oid) : Packet(PACKET_SERVER_ACTION_ON_OBJECT)
    {
        data.t = t;
        data.a = a;
        data.oid = oid;
    }
    PacketServerActionOnObject() : Packet(PACKET_SERVER_ACTION_ON_OBJECT)
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

class PacketPlayerActionUseItemOnTile : public Packet
{
    struct serial_data
    {
        PacketType t;
        uintptr_t iid;
        int map_x;
        int map_y;
        int x;
        int y;
    } data, *pdata __attribute__((packed));

  public:
    int get_map_x() { return pdata->map_x; }
    int get_map_y() { return pdata->map_y; }
    int get_x() { return pdata->x; }
    int get_y() { return pdata->y; }
    uintptr_t get_iid() { return pdata->iid; }

    PacketPlayerActionUseItemOnTile(uintptr_t iid, int map_x, int map_y, int x, int y) : Packet(PACKET_PLAYER_ACTION_USE_ITEM_ON_TILE)
    {
        data.t = t;
        data.iid = iid;
        data.map_x = map_x;
        data.map_y = map_y;
        data.x = x;
        data.y = y;
    }
    PacketPlayerActionUseItemOnTile() : Packet(PACKET_PLAYER_ACTION_USE_ITEM_ON_TILE)
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

class PacketPlayerActionCraft : public Packet
{
    struct serial_data
    {
        PacketType t;
        size_t size;
        uintptr_t prod_id;
        uintptr_t ing_num;
        uintptr_t iid[0];
        static void * operator new(size_t size_base, size_t extra)
        {
            printf("serial_data: allocating %ld + %ld\n", size_base, extra);
            return ::operator new(size_base + extra);
        }
        serial_data(uintptr_t prod_id, uintptr_t ing_num, const uintptr_t *iid_tab, PacketType t) : t(t), prod_id(prod_id), ing_num(ing_num)
        {
            for (uintptr_t i=0; i < ing_num; i++)
            {
                iid[i] = iid_tab[i];
            }
            size = sizeof(struct serial_data) + ing_num * sizeof(*iid_tab);
        }
        static void operator delete(void * ptr)
        {
            ::operator delete(ptr);
        }
    } *pdata __attribute__((packed));

  public:
    uintptr_t get_prod_id() { return pdata->prod_id; }
    uintptr_t get_ing_num() { return pdata->ing_num; }
    uintptr_t * get_iid_table() { return pdata->iid; }

    PacketPlayerActionCraft(uintptr_t prod_id, uintptr_t ing_num, const uintptr_t *iid ) : Packet(PACKET_PLAYER_ACTION_CRAFT)
    {
        pdata = new (ing_num * sizeof(*iid)) serial_data(prod_id, ing_num, iid, t);
    }
    PacketPlayerActionCraft() : Packet(PACKET_PLAYER_ACTION_CRAFT)
    {
    }
    int send(ENetPeer * peer)
    {
        /*for (int i=0; i< 100; i++)
            printf("[%d] = %d %x\n", i, obj->data[i], (obj->data[i]));
*/
        return send_data(peer, pdata, pdata->size);
    }
    bool update(unsigned char * data, size_t s)
    {
        pdata = (struct serial_data *)data;
        if (s != pdata->size)
            return false;
        /*      for (int i=0; i<100; i++)
                    printf("[%d] = %d %x\n", i, pdata->data[i], (pdata->data[i]));
        */
        return true;
    }
};

class PacketKnowledgeUpdate : public Packet
{
    struct serial_data
    {
        PacketType t;
        int pl_id;
        Class_id cid;
        int id;
    } data, *pdata __attribute__((packed));

  public:
    int  get_pl_id()
    {
        return pdata->pl_id;
    }
    Class_id  get_cid()
    {
        return pdata->cid;
    }
    int  get_id()
    {
        return pdata->id;
    }
    PacketKnowledgeUpdate(int pl_id, Class_id cid, int id) : Packet(PACKET_KNOWLEDGE_UPDATE)
    {
        data.t = t;
        data.pl_id=pl_id;
        data.cid=cid;
        data.id=id;
    }
    PacketKnowledgeUpdate() : Packet(PACKET_KNOWLEDGE_UPDATE)
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
};

class PacketCheckedUpdate : public Packet
{
    struct serial_data
    {
        PacketType t;
        int pl_id;
        uintptr_t id;
    } data, *pdata __attribute__((packed));

  public:
    int  get_pl_id()
    {
        return pdata->pl_id;
    }
    uintptr_t get_id()
    {
        return pdata->id;
    }
    PacketCheckedUpdate(int pl_id, uintptr_t id) : Packet(PACKET_CHECKED_UPDATE)
    {
        data.t = t;
        data.pl_id=pl_id;
        data.id=id;
    }
    PacketCheckedUpdate() : Packet(PACKET_CHECKED_UPDATE)
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
};

Packet * check_packet(char dir, unsigned char * data, size_t s)
{    
    Packet * p = nullptr;
    switch (data[0])
    {
        case PACKET_JOIN_REQUEST:    p = new PacketJoinRequest();    break;
        case PACKET_ACTION_FAILED:    p = new PacketActionFailed();    break;
        case PACKET_PLAYER_ID:       p = new PacketPlayerId();       break;
        case PACKET_PLAYER_MOVE:     p = new PacketPlayerMove();     break;
        case PACKET_CHUNK_UPDATE:    p = new PacketChunkUpdate();    break;
        case PACKET_OBJECT_CREATE:   p = new PacketObjectCreate();   break;
        case PACKET_OBJECT_UPDATE:   p = new PacketObjectUpdate();   break;
        case PACKET_OBJECT_DESTROY:   p = new PacketObjectDestroy();   break;
        case PACKET_LOCATION_UPDATE: p = new PacketLocationUpdate(); break;
        case PACKET_REQUEST_CHUNK:   p = new PacketRequestChunk(); break;
        case PACKET_PLAYER_ACTION_PICKUP: p = new PacketPlayerActionPickup(); break;
        case PACKET_PLAYER_ACTION_DROP: p = new PacketPlayerActionDrop(); break;
        case PACKET_PLAYER_ACTION_USE_ITEM_ON_OBJECT: p = new PacketPlayerActionUseItemOnObject(); break;
        case PACKET_PLAYER_ACTION_ON_OBJECT: p = new PacketPlayerActionOnObject(); break;
        case PACKET_SERVER_ACTION_ON_OBJECT: p = new PacketServerActionOnObject(); break;
        case PACKET_PLAYER_ACTION_USE_ITEM_ON_TILE: p = new PacketPlayerActionUseItemOnTile(); break;
        case PACKET_PLAYER_ACTION_CRAFT: p = new PacketPlayerActionCraft(); break;
        case PACKET_KNOWLEDGE_UPDATE: p = new PacketKnowledgeUpdate(); break;
        case PACKET_CHECKED_UPDATE: p = new PacketCheckedUpdate(); break;
        case PACKET_ELEMENTS_LIST: p = new PacketElementsList(); break;
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
