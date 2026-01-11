#include <sys/stat.h>
#include <sys/types.h>
#include <SDL_ttf.h>
#include <errno.h>
#include <stdio.h>

#include "../client-common/text.h"
#include "../client-common/window.h"

TTF_Font * font;

SDL_Color White = {255, 255, 255};
SDL_Color Gray = {200, 200, 200};
SDL_Color Red = {255, 0, 0};
SDL_Color Cyan = {255, 0, 255};
SDL_Color Yellow = {255, 255, 0};


int load_font()
{
    struct stat statbuf;
    int ret;
    ret = stat(FONT_NAME, &statbuf);
    if (ret)
    {
        CONSOLE_LOG("load_font(%s): %s\n", FONT_NAME, strerror(errno));
        return 1;
    }
    font = TTF_OpenFont(FONT_NAME, INITIAL_FONT_SIZE);
    if (!(font))
        return 1;
    else
        return 0;
}

void write_text(int x, int y, const char * text, SDL_Color color, int scale_x, int scale_y, bool clear_bg)
{
    SDL_Surface * surface;
    surface = TTF_RenderText_Solid(font, text, color);
    SDL_Texture * text_sdl = SDL_CreateTextureFromSurface(renderer, surface);

    int window_w, window_h;
    SDL_GetWindowSize(main_window, &window_w, &window_h);

    int game_size;

    if (window_w < window_h)
    {
        game_size = window_w;
    }
    else
    {
        game_size = window_h;
    }

    int x_size, y_size;
    x_size = strlen(text) * (scale_x ? scale_x : game_size / 25);
    y_size = scale_y ? scale_y : game_size / 10;

    SDL_Rect rect = {x, y, x_size, y_size};

    if (clear_bg)
    {
        SDL_SetRenderDrawColor(renderer, 10, 10, 50, 255);
        SDL_RenderFillRect(renderer, &rect);
    }

    SDL_RenderCopy(renderer, text_sdl, NULL, &rect);
    SDL_FreeSurface(surface);
    SDL_DestroyTexture(text_sdl);
}


