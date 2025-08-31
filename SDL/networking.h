#ifndef NETWORKING_H
#define NETWORKING_H

#include "../core/world.h"
#include "../core/player.h"
#include "../core/npc.h"

#ifdef USE_ENET
#include <enet/enet.h>
typedef ENetHost NetClient;
#include "networking_enet.h"
#else
class NetClient
{
};

#include "networking_gen.h"
#endif
class UdpSocket
{
};

extern NetClient * client;

#endif
