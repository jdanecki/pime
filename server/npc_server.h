#ifndef NPC_SERVER_H
#define NPC_SERVER_H

#include "../core/npc.h"
#include "elements/being_server.h"

class NPCServer : public Npc, public BeingServer
{
    enum NpcState
    {
        IDLE,
        FINDING_FOOD,
        DISCOVERING,
        CONVERSATION,
    };
    float dst_x;
    float dst_y;
    int dst_map_x;
    int dst_map_y;
    NpcState state;
    InventoryElement * target;
    void check_state();
    void find_plant();

  public:
    NPCServer(size_t uid);
    void show(bool details = true) override;
    bool tick() override;
    void pickup(InventoryElement * item);
    bool player_action(Player_action action, Player * pl) override;
};

Npc * create_npc();
void show_npcs();
void create_npcs();

#endif // NPC_SERVER_H
