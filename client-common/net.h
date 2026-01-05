#ifndef NET_H
#define NET_H
#include "../core/networking.h"

#define PLAYER_NUM 16
extern Player * players[PLAYER_NUM];
extern ElementsList objects;
extern int active_hotbar;

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
void put_element();

#endif // NET_H
