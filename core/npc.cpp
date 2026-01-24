#include "npc.h"

// defined in SDL/main.h
// #define PLAYER_NUM 16

int npcs_count;

Npc::Npc(size_t uid, ItemLocation location) : Player(uid, SerializableCString(create_name(rand() % 3 + 3)), location, 100, 100, 100)
{
    c_id = Class_Npc;
}
bool Npc::check_known(InventoryElement * el)
{
    return true;
}
