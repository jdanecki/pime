#ifndef PLAYER_TUI
#define PLAYER_TUI

#include "../core/player.h"

class PlayerUI : public Player
{
  public:
    InventoryElement * hotbar[10];
    int craftbar[10];
    PlayerUI(Player data);
};

extern PlayerUI * player;

#endif
