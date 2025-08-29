#include "playerSDL.h"
#include "../texture.h"

SDL_Texture* PlayerSDL::get_texture()
{
    return Texture.player;
}

PlayerSDL::PlayerSDL(Player data) : Player(data) {}
