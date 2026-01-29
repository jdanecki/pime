#include <sys/stat.h>
#include <sys/types.h>
#include <SDL2/SDL_image.h>
#include <errno.h>

#include "../client-common/text.h"
#include "backend.inl"

#define FONT_WIDTH 85
#define FONT_HEIGHT 128
SDL_Texture * font;

SDL_Color White = {255, 255, 255};
SDL_Color Gray = {200, 200, 200};
SDL_Color Red = {255, 0, 0};
SDL_Color Cyan = {255, 0, 255};
SDL_Color Yellow = {255, 255, 0};

int load_font()
{
    struct stat statbuf;
    int ret;
    ret = stat("font.png", &statbuf);
    if (ret)
    {
        CONSOLE_LOG("load_font(%s): %s\n", "font.png", strerror(errno));
        return 1;
    }
    SDL_Surface * surf = IMG_Load("font.png");
    font = SDL_CreateTextureFromSurface(renderer, surf);
    SDL_FreeSurface(surf);
    if (!(font))
        return 1;
    else
        return 0;
}

void write_text(int x, int y, const char * text, SDL_Color color, int scale_x, int scale_y, bool clear_bg)
{
    const char * c = text;
    int cx = x;
    SDL_SetTextureColorMod(font, color.r, color.g, color.b);
    while (*c)
    {
        int src_top = (*c - 32) / 16 * FONT_HEIGHT;
        int src_left = (*c - 32) % 16 * FONT_WIDTH;
        SDL_Rect src_rect = {src_left, src_top, FONT_WIDTH, FONT_HEIGHT};
        SDL_Rect rect = {cx, y, scale_x, scale_y};
        SDL_RenderCopy(renderer, font, &src_rect, &rect);
        c++;
        cx += scale_x;
    }
    SDL_SetTextureColorMod(font, 255, 255, 255);
}
