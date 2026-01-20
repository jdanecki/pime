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
    PlayerClient(PlayerServer * player, ENetPeer * peer);
    bool check(void * what);
    void show(bool details = true);
};

void add_packet_to_send(Packet * p);
void add_packet_to_send1(Packet * p);
void notify_update(const InventoryElement * el);
void notify_create(const InventoryElement * el);
void notify_destroy(InventoryElement * el);
bool init_networking();
void send_updates();
void handle_net_event(ENetEvent * event);

#endif
