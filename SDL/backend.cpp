#include "backend.inl"
#include "../core/alchemist/ncurses-output.h"

SDL_Texture * Backend_Create_Texture_From_Surface(SDL_Surface * image)
{
    return SDL_CreateTextureFromSurface(renderer,image);
}

const char * Backend_Get_Error() {
    return SDL_GetError();
}

void Backend_Wait()
{
  //  SDL_Delay(20);
}

void Backend_Update_Screen()
{
    SDL_RenderPresent(renderer);
}

void Backend_Texture_Copy_With_Mask(SDL_Texture *texture, Backend_Rect *srcrect, Backend_Rect *dstrect, SDL_Color color, bool mask)
{
    if (mask)
    {
        SDL_SetTextureColorMod(texture, color.r, color.g, color.b);
    }
    SDL_Rect * src=srcrect ? &srcrect->r : nullptr;
    SDL_RenderCopy(renderer, texture, src, &dstrect->r);
    //   SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    //   SDL_RenderDrawRect(renderer, &dstrect->r);
}
void Backend_Texture_Copy(SDL_Texture *texture, Backend_Rect *srcrect, Backend_Rect *dstrect)
{
    SDL_Rect * src=srcrect ? &srcrect->r : nullptr;
    SDL_RenderCopy(renderer, texture, src, &dstrect->r);
}

void Backend_Texture_Copy_Flip(SDL_Texture *texture, Backend_Rect *srcrect, Backend_Rect *dstrect)
{
    SDL_Rect * src=srcrect ? &srcrect->r : nullptr;
    SDL_RenderCopyEx(renderer, texture, src, &dstrect->r, 0, NULL, SDL_FLIP_HORIZONTAL);
}

int Backend_Get_Texture_Size(SDL_Texture *texture, int *w, int *h)
{
//    unsigned int format;
  //  int ret=SDL_QueryTexture(texture, &format, NULL, w, h);
    int ret=SDL_QueryTexture(texture, NULL, NULL, w, h);
    if (ret)
        CONSOLE_LOG("SDL_QueryTexture failed: %s", SDL_GetError());
 //   printf("format=%x %s\n", format, SDL_GetPixelFormatName(format));
 //   format=16462004
    return ret;
}

Backend_Pixels Backend_Allocate_Pixels(int w, int h)
{
    Backend_Pixels pixels;
    pixels.texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_ABGR8888, SDL_TEXTUREACCESS_STREAMING, w, h);
    SDL_SetTextureBlendMode(pixels.texture, SDL_BLENDMODE_BLEND);

    SDL_LockTexture(pixels.texture, NULL, (void **)&pixels.pixels, &pixels.pitch);
    return pixels;
}

void Backend_Update_Texture_Pixels(Backend_Pixels pixels)
{
    SDL_UnlockTexture(pixels.texture);
}

void Backend_Draw_Fill_Rectangle(Backend_Rect r, SDL_Color color)
{
    SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);
    SDL_RenderFillRect(renderer, &r.r);
}

void Backend_Draw_Rectangle(Backend_Rect r, SDL_Color color)
{
   SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);
   SDL_RenderDrawRect(renderer, &r.r);
}

void Backend_Window_Size(int *w, int *h)
{
    SDL_GetWindowSize(main_window, w, h);
}

void Backend_Draw_Gradient_Rectangle(int x, int y, int w, int h, SDL_Color top, SDL_Color bottom)
{
    SDL_Vertex v[4];
    int i[6] = {0, 1, 2, 0, 3, 2};
    v[0].position.x = x;
    v[0].position.y = y;

    v[1].position.x = x + w;
    v[1].position.y = y;

    v[2].position.x = x + w;
    v[2].position.y = y + h;

    v[3].position.x = x;
    v[3].position.y = y + h;

    v[0].color = top;
    v[1].color = top;
    v[2].color = bottom;
    v[3].color = bottom;

    v[0].tex_coord.x = 0;
    v[1].tex_coord.x = 0;
    v[2].tex_coord.x = 0;
    v[3].tex_coord.x = 0;

    v[0].tex_coord.y = 0;
    v[1].tex_coord.y = 0;
    v[2].tex_coord.y = 0;
    v[3].tex_coord.y = 0;

    SDL_RenderGeometry(renderer, NULL, v, 6, i, 6);
}
