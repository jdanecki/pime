#ifndef PLAYER_SDL
#define PLAYER_SDL

#include "../core/player.h"
#include "../client-common/2d/alchemist2d.h"

class PlayerUI : public Player, public Renderable
{
  public:
    InventoryElement * hotbar[10];
    int craftbar[10];
    SDL_Texture * get_texture() override;
    PlayerUI(Player data);
    void update_item_location(ItemLocation & old_loc, ItemLocation & new_loc) override;
};

extern PlayerUI * player;

#endif
