#include "packet_types.h"
#include <cstdio>
#include <cassert>

const char * packet_type_names[]={ 
"PACKET_UNKNOWN",
"PACKET_PLAYER_UPDATE",
"PACKET_PLAYER_ID",
"PACKET_PLAYER_MOVE",
"PACKET_CHUNK_UPDATE",
"PACKET_JOIN_REQUEST",
"PACKET_OBJECT_UPDATE",
"PACKET_PLAYER_ACTION_PICKUP",
"PACKET_PLAYER_ACTION_DROP",
"PACKET_PLAYER_ACTION_USE_ITEM_ON_OBJECT",
"PACKET_PLAYER_ACTION_CRAFT",
"PACKET_LOCATION_UPDATE",
"PACKET_OBJECT_CREATE",
"PACKET_OBJECT_DESTROY",
"PACKET_FAILED_CRAFT",
"PACKET_PLAYER_ACTION_USE_ITEM_ON_TILE",
"PACKET_REQUEST_CHUNK",
"PACKET_KEEP_ALIVE",
"PACKET_PLAYER_ACTION_ON_OBJECT",
"PACKET_SERVER_ACTION_ON_OBJECT",
"PACKET_ACTION_FAILED",
"PACKET_KNOWLEDGE_UPDATE",
"PACKET_CHECKED_UPDATE",
"PACKET_COUNT",
};

int trace_network;

void show_packet_type_name(char what, unsigned char i)
{
    trace_network %= 4;
    switch (trace_network)
    {
        case 3: if (i == PACKET_PLAYER_UPDATE || i == PACKET_KEEP_ALIVE) return;
        case 2: if (i == PACKET_PLAYER_UPDATE) return;
        case 1: 
            printf("%c %d -> %s\n", what, i, i < PACKET_COUNT ? packet_type_names[i] : "ERROR" ); break;
        default: break;
    }  
    assert(i < PACKET_COUNT);
}

