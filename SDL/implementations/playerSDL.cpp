#include "playerSDL.h"
#include "../texture.h"

SDL_Texture * PlayerSDL::get_texture()
{
    return Player_textures.player;
}

PlayerSDL::PlayerSDL(Player data) : Player(data)
{
    for (int i = 0; i < 10; i++)
    {
        hotbar[i] = NULL;
        craftbar[i] = 0;
    }
}
