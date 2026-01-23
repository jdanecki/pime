#include <new>

#include "packets.h"

int Packet::send_data(ENetPeer * peer, void * data, size_t size)
{
    show_packet_type_name('S', *(unsigned char *)data);
    ENetPacket * p = enet_packet_create(data, size, ENET_PACKET_FLAG_RELIABLE);
    int ret = enet_peer_send(peer, 0, p);
    enet_host_flush(peer->host);
    return ret;
}
Packet::Packet(PacketType t) : t(t)
{
}
Packet::~Packet()
{
}
int Packet::send(ENetPeer * peer)
{
    return send_data(peer, &t, sizeof(PacketType));
}
PacketType Packet::get_type()
{
    return t;
}

bool Packet::update(unsigned char * data, size_t s)
{
    return s == sizeof(PacketType);
}
PacketJoinRequest::PacketJoinRequest() : Packet(PACKET_JOIN_REQUEST)
{
}
PacketActionFailed::PacketActionFailed() : Packet(PACKET_ACTION_FAILED)
{
}

unsigned long PacketPlayerId::get_id()
{
    return pdata->id;
}
