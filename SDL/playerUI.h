#ifndef PLAYER_SDL
#define PLAYER_SDL

#include "../core/player.h"
#include "../client-common/alchemist2d.h"

class PlayerUI : public Player, public Renderable
{
  public:
    bool going_right;
    InventoryElement * hotbar[10];
    int craftbar[10];
    SDL_Texture * get_texture() override;
    PlayerUI(Player data);
    void render(int x, int y) override;
    void update_item_location(ItemLocation & old_loc, ItemLocation & new_loc) override;
};

extern PlayerUI * player;

#endif
