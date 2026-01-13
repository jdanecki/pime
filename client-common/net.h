#ifndef NET_H
#define NET_H
#include "../core/networking.h"

class NetClient
{
  public:
    ENetHost * host;
    ENetPeer * peer;
    NetClient(ENetHost * host, ENetPeer * peer) : host(host), peer(peer)
    {
    }
};
extern NetClient * client;

extern ElementsList objects;
extern chunk * check_chunk(int cx, int cy);

void send_packet_move(NetClient * client, int32_t x, int32_t y);
void send_packet_pickup(NetClient * client, uintptr_t id);
void send_packet_drop(NetClient * client, uintptr_t id);
void send_packet_item_used_on_object(NetClient * client, uintptr_t iid, uintptr_t oid);
void send_packet_action_on_object(NetClient * client, int32_t a, uintptr_t oid);
void send_packet_server_action_on_object(NetClient * client, int32_t a, uintptr_t oid);
void send_packet_item_used_on_tile(NetClient * client, uintptr_t iid, ItemLocation location);
void send_packet_craft(NetClient * client, uintptr_t prod_id, uintptr_t ingredients_num, const uintptr_t * iid);
void send_packet_request_chunk(NetClient * client, int32_t x, int32_t y);
void send_packet_request_item(NetClient * client, size_t id);
void server_action_tile(Server_action a, ItemLocation loc);

NetClient * init(const char * server_ip, const char * port);

uint32_t network_tick(NetClient * client);

InventoryElement * get_object_by_id(NetworkObject uid);

void register_object(NetworkObject * o);

void deregister_object(NetworkObject * o);

BaseElement * get_base_element(size_t id);

BasePlant * get_base_plant(size_t id);

BaseAnimal * get_base_animal(size_t id);

Base * get_base(uint32_t c_id, int32_t id);

extern void update_chunk(int32_t x, int32_t y, const chunk_table * data);

extern void update_object(const ObjectData * data);

extern void update_item_location(LocationUpdateData data);

extern void create_object(const ObjectData * data);

extern void destroy_object(NetworkObject id, ItemLocation location);

extern void failed_craft();

extern void action_failed();

extern void knowledge_update(size_t pl_id, Class_id cid, int32_t id);

extern void checked_update(size_t pl_id, uintptr_t el);

extern NetworkObject * el_from_data(const ObjectData * data);

#endif // NET_H
