#ifndef NET_H
#define NET_H
#include "../core/networking.h"

class NetClient
{
  public:
    ENetHost * host;
    ENetPeer * peer;
    NetClient(ENetHost * host, ENetPeer * peer);
};
extern const char * ip;
extern const char * port;

extern ElementsList objects;

extern chunk * check_chunk(int cx, int cy);

void send_packet_move(int32_t x, int32_t y);
void send_packet_pickup(uintptr_t id);
void send_packet_drop(uintptr_t id);
void send_packet_item_used_on_object(uintptr_t iid, uintptr_t oid);
void send_packet_action_on_object(int32_t a, uintptr_t oid);
void send_packet_server_action_on_object(int32_t a, uintptr_t oid);
void send_packet_item_used_on_tile(uintptr_t iid, ItemLocation location);
void send_packet_craft(uintptr_t prod_id, uintptr_t ingredients_num, const uintptr_t * iid);
void send_packet_request_chunk(int32_t x, int32_t y);
void send_packet_request_item(size_t id);
void server_action_tile(Server_action a, ItemLocation loc);

bool init_networking();

uint32_t network_tick();

InventoryElement * get_object_by_id(NetworkObject uid);

void register_object(NetworkObject * o);

void deregister_object(NetworkObject * o);

BaseElement * get_base_element(size_t id);

BasePlant * get_base_plant(size_t id);

BaseAnimal * get_base_animal(size_t id);

Base * get_base(uint32_t c_id, int32_t id);

// Client should implement: (or use net2d.cpp)
void got_id(size_t id, int64_t seed);
void update_chunk(int32_t x, int32_t y, const chunk_table * data);
void update_object(const ObjectData * data);
void update_item_location(LocationUpdateData data);
void create_object(const ObjectData * data);
void destroy_object(NetworkObject id, ItemLocation location);
void failed_craft();
void action_failed();
void knowledge_update(size_t pl_id, Class_id cid, int32_t id);
void checked_update(size_t pl_id, uintptr_t el);
NetworkObject * el_from_data(const ObjectData * data);
void connect();
void disconnect();

#endif // NET_H
