#ifndef PACKET_TYPE_H
#define PACKET_TYPE_H

using PacketType = unsigned char;
constexpr static const PacketType PACKET_UNKNOWN = 0;
constexpr static const PacketType PACKET_PLAYER_UPDATE = 1;
constexpr static const PacketType PACKET_PLAYER_ID = 2;
constexpr static const PacketType PACKET_PLAYER_MOVE = 3;
constexpr static const PacketType PACKET_CHUNK_UPDATE = 4;
constexpr static const PacketType PACKET_JOIN_REQUEST = 5;
constexpr static const PacketType PACKET_OBJECT_UPDATE = 6;
constexpr static const PacketType PACKET_PLAYER_ACTION_PICKUP = 7;
constexpr static const PacketType PACKET_PLAYER_ACTION_DROP = 8;
constexpr static const PacketType PACKET_PLAYER_ACTION_USE_ITEM_ON_OBJECT = 9;
constexpr static const PacketType PACKET_PLAYER_ACTION_CRAFT = 10;
constexpr static const PacketType PACKET_LOCATION_UPDATE = 11;
constexpr static const PacketType PACKET_OBJECT_CREATE = 12;
constexpr static const PacketType PACKET_OBJECT_DESTROY = 13;
constexpr static const PacketType PACKET_FAILED_CRAFT = 14;
constexpr static const PacketType PACKET_PLAYER_ACTION_USE_ITEM_ON_TILE = 15;
constexpr static const PacketType PACKET_REQUEST_CHUNK = 16;
constexpr static const PacketType PACKET_KEEP_ALIVE = 17;
constexpr static const PacketType PACKET_PLAYER_ACTION_ON_OBJECT = 18;
constexpr static const PacketType PACKET_SERVER_ACTION_ON_OBJECT = 19;
constexpr static const PacketType PACKET_ACTION_FAILED = 20;
constexpr static const PacketType PACKET_KNOWLEDGE_UPDATE = 21;
constexpr static const PacketType PACKET_COUNT = 22;

extern const char * packet_type_names[];
void show_packet_type_name(char what, unsigned char i);
extern bool trace_network;
#endif
