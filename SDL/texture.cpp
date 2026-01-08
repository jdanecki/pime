#include <SDL_surface.h>
#include <SDL_image.h>
#include <SDL_blendmode.h>
#include <SDL_image.h>
#include <dirent.h>
#include <stdio.h>
#include <assert.h>
#include <stdlib.h>

#include "../client-common/window.h"
#include "../client-common/texture.h"

SDL_Texture * add_texture_color(SDL_Surface * source, ColorRGB c)
{
    unsigned char mask_r = c.r;
    unsigned char mask_g = c.g;
    unsigned char mask_b = c.b;

    SDL_Surface * surface = SDL_CreateRGBSurfaceWithFormat(0, 32, 32, 32, SDL_PIXELFORMAT_RGBA8888);
    SDL_BlitSurface(source, NULL, surface, NULL);
    SDL_LockSurface(surface);

    unsigned int * pixels = (unsigned int *)surface->pixels;

    for (int y = 0; y < surface->h; ++y)
    {
        for (int x = 0; x < surface->w; ++x)
        {
            unsigned int * pixel = pixels + y * surface->w + x;

            unsigned char a = *pixel & 0xFF;
            unsigned char b = (*pixel >> 8) & 0xFF;
            unsigned char g = (*pixel >> 16) & 0xFF;
            unsigned char r = (*pixel >> 24) & 0xFF;

            if (r == g && g == b)
            {
                unsigned char gray = r;
                unsigned char nr = (mask_r * gray) / 255;
                unsigned char ng = (mask_g * gray) / 255;
                unsigned char nb = (mask_b * gray) / 255;
                *pixel = (nr << 24) | (ng << 16) | (nb << 8) | a;
            }
        }
    }

    SDL_UnlockSurface(source);
    SDL_Texture * retval = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_FreeSurface(surface);
    return retval;
}
