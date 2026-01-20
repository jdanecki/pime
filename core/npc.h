#ifndef NPC_H
#define NPC_H

#include "player.h"

class Npc : public Player
{
  public:
    Npc(ItemLocation location);
    bool check_known(InventoryElement * el);
};

#endif // NPC_H
