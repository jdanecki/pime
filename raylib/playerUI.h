#ifndef PLAYER_TUI
#define PLAYER_TUI

#include "../core/player.h"
#include "../client-common/alchemist2d.h"

class PlayerUI : public Player, public Renderable
{
  public:
    InventoryElement * hotbar[10];
    int craftbar[10];
    Texture get_texture();
    PlayerUI(Player data);
};

extern PlayerUI * player;

#endif
