#ifndef _PACKETS_H
#define _PACKETS_H

#include <enet/enet.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include "packet_types.h"
#include "chunk.h"
#include "world.h"
#include "networking.h"

extern ElementsList base_elements;
extern ElementsList base_plants;
extern ElementsList base_animals;

class Packet
{
  protected:
    PacketType t;
    int send_data(ENetPeer * peer, void * data, size_t size);

  public:
    Packet(PacketType t);
    virtual ~Packet();
    virtual int send(ENetPeer * peer);
    PacketType get_type();
    virtual bool update(unsigned char * data, size_t s);
};

class PacketJoinRequest : public Packet
{
  public:
    PacketJoinRequest();
};

class PacketActionFailed : public Packet
{
  public:
    PacketActionFailed();
};

class PacketPlayerId : public Packet
{
    struct serial_data
    {
        PacketType t;
        unsigned long id;
    } data, *pdata __attribute__((packed));

  public:
    PacketPlayerId(unsigned long id);
    PacketPlayerId();
    bool update(unsigned char * net_data, size_t s);
    int send(ENetPeer * peer);
    unsigned long get_id();
};

class PacketObjectDestroy : public Packet
{
    struct serial_data
    {
        PacketType t;
        unsigned long id;
        ItemLocation location;
    } data, *pdata __attribute__((packed));
    InventoryElement * el_to_remove;

  public:
    unsigned long get_id();
    ItemLocation get_location();
    PacketObjectDestroy(InventoryElement * el);
    PacketObjectDestroy();
    bool update(unsigned char * net_data, size_t s);
    int send(ENetPeer * peer);
};

extern void add_packet_to_send1(Packet * p);
class PacketElementsList : public Packet
{
    struct serial_data
    {
        PacketType t;
        size_t size;
        int nr_elements;
        char name[60];
        Class_id c_id;
        Class_id list_c_id;
        int pl_id;
        unsigned char data[0];
        static void * operator new(size_t size_base, size_t extra);
        serial_data(size_t s);
        static void operator delete(void * ptr);
    } * pdata __attribute__((packed));

  public:
    int get_nr_elements();
    Class_id get_c_id();
    Class_id get_list_c_id();
    int get_pl_id();
    unsigned char * get_data();

    void copy_base_list_element(ListElement * el, serial_data * pdata, int i);

    void copy_list_element(ListElement * el, serial_data * pdata, int i);
    void init(ElementsList * list);
    PacketElementsList(Player * pl);
    PacketElementsList(ElementsList * list);
    PacketElementsList();
    int send(ENetPeer * peer);
    bool update(unsigned char * data, size_t s);
};

ObjectData * convert_to_data(NetworkObject * el);

class PacketObjectCreate : public Packet
{
    struct serial_data
    {
        PacketType t;
        size_t size;
        unsigned char data[0];
        static void * operator new(size_t size_base, size_t extra);
        serial_data(size_t s);
        static void operator delete(void * ptr);
    } __attribute__((packed));

  public:
    ObjectData * obj;
    PacketObjectCreate(NetworkObject * el);
    PacketObjectCreate();
    int send(ENetPeer * peer);
    bool update(unsigned char * data, size_t s);
};

class PacketObjectUpdate : public Packet
{
    struct serial_data
    {
        PacketType t;
        size_t size;
        unsigned char data[0];
        static void * operator new(size_t size_base, size_t extra);
        serial_data(size_t s);
        static void operator delete(void * ptr);
    } __attribute__((packed));

  public:
    ObjectData * obj;
    PacketObjectUpdate(InventoryElement * el);
    PacketObjectUpdate();
    int send(ENetPeer * peer);
    bool update(unsigned char * data, size_t s);
};

class PacketChunkUpdate : public Packet
{
    bool chunk_valid;
    struct serial_data
    {
        PacketType t;
        unsigned char x, y;
        chunk_table table;
    } data, *pdata __attribute__((packed));
    chunk_table * ptable;

  public:
    unsigned char get_x();
    unsigned char get_y();
    chunk_table * get_table();
    PacketChunkUpdate(unsigned char x, unsigned char y);
    PacketChunkUpdate();
    int send(ENetPeer * peer);
    bool update(unsigned char * net_data, size_t s);
};

class PacketPlayerMove : public Packet
{
    struct serial_data
    {
        PacketType t;
        float x, y;
    } data, *pdata __attribute__((packed));

  public:
    PacketPlayerMove(float x, float y);
    PacketPlayerMove();
    bool update(unsigned char * net_data, size_t s);
    int send(ENetPeer * peer);
    float get_x();
    float get_y();
};

class PacketLocationUpdate : public Packet
{
    struct serial_data
    {
        PacketType t;
        LocationUpdateData location;
    } data, *pdata __attribute__((packed));

  public:
    LocationUpdateData get_location();

    PacketLocationUpdate(size_t uid, ItemLocation old_loc, ItemLocation new_loc);
    PacketLocationUpdate();
    bool update(unsigned char * net_data, size_t s);
    int send(ENetPeer * peer);
};

class PacketRequestChunk : public Packet
{
    struct serial_data
    {
        PacketType t;
        int cx, cy;
    } data, *pdata __attribute__((packed));

  public:
    int get_cx();
    int get_cy();

    PacketRequestChunk(int cx, int cy);
    PacketRequestChunk();
    bool update(unsigned char * net_data, size_t s);
    int send(ENetPeer * peer);
};

class PacketRequestItem : public Packet
{
    struct serial_data
    {
        PacketType t;
        size_t id;
    } data, *pdata __attribute__((packed));

  public:
    int get_id();

    PacketRequestItem(size_t id);
    PacketRequestItem();
    bool update(unsigned char * net_data, size_t s);
    int send(ENetPeer * peer);
};

class PacketPlayerActionPickup : public Packet
{
    struct serial_data
    {
        PacketType t;
        uintptr_t id;
    } data, *pdata __attribute__((packed));

  public:
    uintptr_t get_id();

    PacketPlayerActionPickup(uintptr_t id);
    PacketPlayerActionPickup();
    bool update(unsigned char * net_data, size_t s);
    int send(ENetPeer * peer);
};

class PacketPlayerActionDrop : public Packet
{
    struct serial_data
    {
        PacketType t;
        uintptr_t id;
    } data, *pdata __attribute__((packed));

  public:
    uintptr_t get_id();

    PacketPlayerActionDrop(uintptr_t id);
    PacketPlayerActionDrop();
    bool update(unsigned char * net_data, size_t s);
    int send(ENetPeer * peer);
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
    uintptr_t get_iid();
    uintptr_t get_oid();

    PacketPlayerActionUseItemOnObject(uintptr_t iid, uintptr_t oid);
    PacketPlayerActionUseItemOnObject();
    bool update(unsigned char * net_data, size_t s);
    int send(ENetPeer * peer);
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
    Player_action get_a();
    uintptr_t get_oid();

    PacketPlayerActionOnObject(Player_action a, uintptr_t oid);
    PacketPlayerActionOnObject();
    bool update(unsigned char * net_data, size_t s);
    int send(ENetPeer * peer);
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
    Server_action get_a();
    uintptr_t get_oid();

    PacketServerActionOnObject(Server_action a, uintptr_t oid);
    PacketServerActionOnObject();
    bool update(unsigned char * net_data, size_t s);
    int send(ENetPeer * peer);
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
    int get_map_x();
    int get_map_y();
    int get_x();
    int get_y();
    uintptr_t get_iid();

    PacketPlayerActionUseItemOnTile(uintptr_t iid, int map_x, int map_y, int x, int y);
    PacketPlayerActionUseItemOnTile();
    bool update(unsigned char * net_data, size_t s);
    int send(ENetPeer * peer);
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
        static void * operator new(size_t size_base, size_t extra);
        serial_data(uintptr_t prod_id, uintptr_t ing_num, const uintptr_t * iid_tab, PacketType t);
        static void operator delete(void * ptr);
    } * pdata __attribute__((packed));

  public:
    uintptr_t get_prod_id();
    uintptr_t get_ing_num();
    uintptr_t * get_iid_table();

    PacketPlayerActionCraft(uintptr_t prod_id, uintptr_t ing_num, const uintptr_t * iid);
    PacketPlayerActionCraft();
    int send(ENetPeer * peer);
    bool update(unsigned char * data, size_t s);
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
    int get_pl_id();
    Class_id get_cid();
    int get_id();
    PacketKnowledgeUpdate(int pl_id, Class_id cid, int id);
    PacketKnowledgeUpdate();
    bool update(unsigned char * net_data, size_t s);
    int send(ENetPeer * peer);
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
    int get_pl_id();
    uintptr_t get_id();
    PacketCheckedUpdate(int pl_id, uintptr_t id);
    PacketCheckedUpdate();
    bool update(unsigned char * net_data, size_t s);
    int send(ENetPeer * peer);
};

Packet * check_client_packet(char dir, unsigned char * data, size_t s);
Packet * check_server_packet(char dir, unsigned char * data, size_t s);

#define NOT_SUPPORTED_SEND_METHOD(class_name)                                                                                                                                                          \
    int class_name::send(ENetPeer * peer)                                                                                                                                                              \
    {                                                                                                                                                                                                  \
        assert(0);                                                                                                                                                                                     \
        return 0;                                                                                                                                                                                      \
    }
#define NOT_SUPPORTED_UPDATE_METHOD(class_name)                                                                                                                                                        \
    bool class_name::update(unsigned char * net_data, size_t s)                                                                                                                                        \
    {                                                                                                                                                                                                  \
        assert(0);                                                                                                                                                                                     \
        return false;                                                                                                                                                                                  \
    }

#define CHECK_SIZE(s) (s == sizeof(struct serial_data))

#endif
