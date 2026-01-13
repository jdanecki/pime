#ifndef NETWORKING_CORE_H
#define NETWORKING_CORE_H

#include "../core/packets.h"
#include "player_server.h"

extern ENetHost * server;

class Networked
{
  public:
    Networked();
};

struct Peer_id
{
    enum class Tag
    {
        Peer,
        Id,
    };
    Tag tag;
    union
    {
        ENetPeer * peer;
        unsigned long id;
    };
};

class PlayerClient : public ListElement
{
  public:
    PlayerServer * player;
    ENetPeer * peer;
    PlayerClient(PlayerServer * player, ENetPeer * peer) : player(player), peer(peer)
    {
        show();
    }
    bool check(void * what)
    {
        Peer_id * p = (Peer_id *)what;
        switch (p->tag)
        {
            case Peer_id::Tag::Peer:
                return p->peer == peer;
            case Peer_id::Tag::Id:
                return p->id == player->uid;
        }
        return false;
    }
    void show(bool details = true)
    {
        char hostname[64];
        enet_address_get_host_ip(&peer->address, hostname, 64);
        CONSOLE_LOG("player uid=%ld host=%s port=%u\n", player->uid, hostname, peer->address.port);
        player->location.show();
    }
};

void add_packet_to_send(Packet * p);
void add_packet_to_send1(Packet * p);
void notify_update(const InventoryElement * el);
void notify_create(const InventoryElement * el);
bool init_networking();
void send_updates();
bool handle_packet(ENetPacket * packet, ENetPeer * peer);

#endif
