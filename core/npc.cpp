#include "npc.h"

// defined in SDL/main.h
// #define PLAYER_NUM 16

int npcs_count;

Npc::Npc(ItemLocation location) : Player((size_t)this, SerializableCString(create_name(rand() % 3 + 3)), location, 0, 0, 0)
{
    c_id = Class_Npc;
}
