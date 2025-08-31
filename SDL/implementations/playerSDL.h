#ifndef PLAYER_SDL
#define PLAYER_SDL

#include "../../core/player.h"
#include "alchemistSDL.h"

class PlayerSDL : public Player, public Renderable
{
  public:
    SDL_Texture * get_texture();
    PlayerSDL(Player data);
};

#endif
