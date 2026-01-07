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

SDL_Texture * add_texture_color(SDL_Surface * s, ColorRGB c)
{
    unsigned char mask_r = c.r;
    unsigned char mask_g = c.g;
    unsigned char mask_b = c.b;

    SDL_Surface * surface = SDL_CreateRGBSurfaceWithFormat(0, 32, 32, 32, SDL_PIXELFORMAT_ABGR8888);
    SDL_BlitSurface(s, NULL, surface, NULL);
    SDL_LockSurface(surface);

    unsigned int * pixels = (unsigned int *)surface->pixels;

    for (int y = 0; y < surface->h; ++y)
    {
        for (int x = 0; x < surface->w; ++x)
        {
            unsigned int * pixel = pixels + y * surface->w + x;

            unsigned char r = *pixel & 0xFF;
            unsigned char g = (*pixel >> 8) & 0xFF;
            unsigned char b = (*pixel >> 16) & 0xFF;
            unsigned char a = (*pixel >> 24) & 0xFF;

            if (r == g && g == b)
            {
                unsigned char gray = r;
                unsigned char nr = (mask_r * gray) / 255;
                unsigned char ng = (mask_g * gray) / 255;
                unsigned char nb = (mask_b * gray) / 255;
                *pixel = (a << 24) | (nb << 16) | (ng << 8) | nr;
            }
        }
    }

    SDL_UnlockSurface(s);
    SDL_Texture * retval = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_FreeSurface(surface);
    return retval;
}
