#include <time.h>
#include <SDL.h>
#include <SDL_image.h>
#include <SDL_ttf.h>
#include <SDL2_gfxPrimitives.h>
#include <sys/stat.h>
#include <unistd.h>

#include "../core/time_core.h"

#include "../client-common/text.h"
#include "../client-common/window.h"
#include "../client-common/texture.h"

SDL_Renderer * renderer;
SDL_Window * main_window;
int window_width;
int window_height;

int init_window(const char * title, int wx, int wy)
{
    Uint32 flags;
    // flags = SDL_WINDOW_RESIZABLE | SDL_WINDOW_HIDDEN;
    flags = SDL_WINDOW_HIDDEN;
    unsigned long t1 = get_time_usec();
//    if (SDL_Init(SDL_INIT_AUDIO | SDL_INIT_VIDEO) < 0)
    if (SDL_Init(SDL_INIT_VIDEO) < 0)
    {
        fprintf(stderr, "\nUnable to initialize SDL:  %s\n", SDL_GetError());
    }

    unsigned long t2 = get_time_usec();
    CONSOLE_LOG("Time it took to initialize SDL2: %ldms\n", (t2 - t1) / 1000);

    if (SDL_CreateWindowAndRenderer(0, 0, flags, &main_window, &renderer) < 0)
    {
        SDL_Log("SDL_CreateWindowAndRenderer() failed: %s\n", SDL_GetError());
        return 1;
    }
    SDL_SetWindowTitle(main_window, title);
    SDL_SetWindowPosition(main_window, 150, 10);
    SDL_SetWindowSize(main_window, wx, wy);
    SDL_GetWindowSize(main_window, &window_width, &window_height);
    CONSOLE_LOG("window_width=%d window_height=%d\n", window_width, window_height);
    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
    SDL_ShowWindow(main_window);

    int imgFlags = IMG_INIT_PNG;
    if (!(IMG_Init(imgFlags) & imgFlags))
    {
        CONSOLE_LOG("\nUnable to initialize sdl_image:  %s\n", IMG_GetError());
        return 1;
    }

    SDL_Surface * icon = IMG_Load("textures/pime.png");
    if (icon == NULL)
    {
        CONSOLE_LOG("\nUnable to load image %s! SDL_image Error: %s\n", "textures/pime.png", IMG_GetError());
        return 1;
    }
    SDL_SetWindowIcon(main_window, icon);
    SDL_FreeSurface(icon);

    TTF_Init();

    if (load_font()) return 1;

    struct stat statbuf;
    int ret = stat("textures", &statbuf);
    if (ret)
    {
        chdir("..");
        ret = stat("textures", &statbuf);
        if (ret)
        {
            CONSOLE_LOG("missing directory with textures\n");
            return 2;
        }
    }

    load_textures();

    unsigned long t3 = get_time_usec();
    CONSOLE_LOG("Time it took to initialize SDL2 modules (img, window, renderer): %ldms\n ", (t3 - t1) / 1000);

    return 0;
}

void close_graphics()
{
    SDL_Quit();
}

void clear_window()
{
    SDL_SetRenderDrawColor(renderer, 10, 10, 50, 255);
    SDL_RenderClear(renderer);
}

unsigned int color(int r, int g, int b, int a)
{
    return (a << 24) | (b << 16) | (g << 8) | r;
}

void update_window_size()
{
    int width;
    SDL_GetWindowSize(main_window, &window_width, &window_height);

    width = window_width - PANEL_WINDOW;

    if (width < window_height)
    {
        tile_size = width / CHUNK_SIZE;
    }
    else
    {
        tile_size = window_height / CHUNK_SIZE;
    }
    if (tile_size < 32)
        tile_size = 32;

    SDL_SetWindowSize(main_window, (tile_size * CHUNK_SIZE) + PANEL_WINDOW, tile_size * CHUNK_SIZE + STATUS_LINES);
    SDL_GetWindowSize(main_window, &window_width, &window_height);
}
