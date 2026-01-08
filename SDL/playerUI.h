#ifndef PLAYER_SDL
#define PLAYER_SDL

#include "../core/player.h"
#include "../client-common/alchemist2d.h"

class PlayerUI : public Player, public Renderable
{
  public:
    InventoryElement * hotbar[10];
    int craftbar[10];
    SDL_Texture * get_texture();
    PlayerUI(Player data);
};

extern PlayerUI* player;

#endif
