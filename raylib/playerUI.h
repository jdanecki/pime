#ifndef PLAYER_RAYLIB
#define PLAYER_RAYLIB

#include "../core/player.h"
#include "../client-common/alchemist2d.h"
#include "backend.inl"

class PlayerUI : public Player, public Renderable
{
  public:
    bool going_right;
    InventoryElement * hotbar[10];
    int craftbar[10];
    Backend_Texture get_texture() override;
    PlayerUI(Player data);
    void update_item_location(ItemLocation & old_loc, ItemLocation & new_loc) override;
};

extern PlayerUI * player;

#endif
