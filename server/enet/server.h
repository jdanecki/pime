#include "../../core/packet_types.h"

struct Player_ID
{
    PacketType t;
    unsigned int id;
} __attribute__((packed)) ;

