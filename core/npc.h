#ifndef NPC_H
#define NPC_H

#include "player.h"

class Npc : public Player
{
  public:
    Npc();
    bool check_known(InventoryElement * el)
    {
        return true;
    }
};

#endif // NPC_H
