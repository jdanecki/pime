#ifndef PLAYER_TUI
#define PLAYER_TUI

#include "../core/player.h"

class PlayerTUI : public Player
{
  public:
    InventoryElement * hotbar[10];
    int craftbar[10];
    PlayerTUI(Player data);
};

extern PlayerTUI* player;

#endif
