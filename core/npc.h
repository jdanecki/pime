#ifndef NPC_H
#define NPC_H

#include "player.h"

class Npc : public Player
{
      public:
    Npc(size_t uid, ItemLocation location);
    bool check_known(InventoryElement * el);
    void ask(enum Npc_say s, InventoryElement * el);
    bool say(Sentence * s);
    Sentence * get_answer(Sentence * s);
    void show(bool details = true);
};

#endif // NPC_H
