#include "backend.inl"

SDL_Texture * Backend_Create_Texture_From_Surface(SDL_Surface * image)
{
    return SDL_CreateTextureFromSurface(renderer,image);
}

const char * Backend_Get_Error() {
    return SDL_GetError();
}
