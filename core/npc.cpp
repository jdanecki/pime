#include "npc.h"

// defined in SDL/main.h
// #define PLAYER_NUM 16

int npcs_count;

Npc::Npc() : Player(16 + (++npcs_count))
{
    c_id = Class_Npc;
    name = create_name(rand() % 3 + 3);

}
