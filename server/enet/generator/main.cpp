#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#include "../../../SDL/text.h"

#define CHUNK_SIZE 16
#define REGIONS_NUM 100
#define WORLD_SIZE 256

#include "generator.cpp"

Region **regions;

SDL_Renderer * renderer;
SDL_Window * main_window;
int window_width;
int window_height;

unsigned long get_time_usec()
{
    struct timespec t;

    clock_gettime(CLOCK_MONOTONIC_RAW, &t);
    return (t.tv_sec * 1000000 + t.tv_nsec / 1000);
}

SDL_Texture * map;

void draw_maps()
{
    unsigned int * pixels;
    int pitch, x, y;

    SDL_LockTexture(map, NULL, (void **)&pixels, &pitch);

    for (y = 0; y < WORLD_SIZE; y++)
    {
        for (x = 0; x < WORLD_SIZE; x++)
        {
            Region * r = find_region(x, y);
            {
                int color = r->terrain_type->id * 12;
                unsigned long c = 0xff000000;
                switch(r->terrain_type->form)
                {
                    case 1: c += color; break;
                    case 2: c += color << 8; break;
                    case 3: c += color << 16; break;
                }
                pixels[y * WORLD_SIZE + x] = c;
            }

        }
    }

    SDL_UnlockTexture(map);

}


int init_window()
{
    Uint32 flags;
    // flags = SDL_WINDOW_RESIZABLE | SDL_WINDOW_HIDDEN;
    flags = SDL_WINDOW_HIDDEN;
    unsigned long t1 = get_time_usec();
    if (SDL_Init(SDL_INIT_AUDIO | SDL_INIT_VIDEO) < 0)
    {
        fprintf(stderr, "\nUnable to initialize SDL:  %s\n", SDL_GetError());
    }

    unsigned long t2 = get_time_usec();
    printf("Time it took to initialize SDL2: %ldms\n", (t2 - t1) / 1000);

    if (SDL_CreateWindowAndRenderer(0, 0, flags, &main_window, &renderer) < 0)
    {
        SDL_Log("SDL_CreateWindowAndRenderer() failed: %s\n", SDL_GetError());
        return 1;
    }
    SDL_SetWindowTitle(main_window, "pime");
    SDL_SetWindowPosition(main_window, 150, 10);
    SDL_SetWindowSize(main_window, 512, 256 + 64);
    SDL_GetWindowSize(main_window, &window_width, &window_height);
    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
    SDL_ShowWindow(main_window);

    int imgFlags = IMG_INIT_PNG;
    if (!(IMG_Init(imgFlags) & imgFlags))
    {
        printf("\nUnable to initialize sdl_image:  %s\n", IMG_GetError());
        return 1;
    }

    TTF_Init();
    load_font();
    unsigned long t3 = get_time_usec();
    printf("Time it took to initialize SDL2 modules (img, window, renderer): %ldms\n ", (t3 - t1) / 1000);

    return 0;
}

void clear_window()
{
    SDL_SetRenderDrawColor(renderer, 10, 10, 50, 255);
    SDL_RenderClear(renderer);
}

int main() {
    srand((unsigned int)time(NULL));

    create_regions();

    /*for (int i = 0; i < 3; i++)
    {
        r[i]->show();
    }
*/
    init_window();
    map = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_ABGR8888, SDL_TEXTUREACCESS_STREAMING, WORLD_SIZE, WORLD_SIZE);
    draw_maps();
    int mx=0, my=0;

    for(;;)
    {
        clear_window();
        SDL_Event event;
        while (SDL_PollEvent(&event))
        {
            if (event.type == SDL_QUIT)
            {
                goto end;
            }

            if (event.type == SDL_KEYDOWN)
            {
                int key = event.key.keysym.sym;
                switch(key)
                {
                    case SDLK_ESCAPE: goto end;
                    case 'g':
                    {
                        for (int i = 0; i < REGIONS_NUM; i++)
                            delete regions[i];
                        for (int i = 0; i < terrains_count; i++)
                            delete terrains[i];

                        create_regions();
                        draw_maps();
                    }
                }
            }
            if (event.type == SDL_MOUSEMOTION)
            {
                print_status(0, "mouse %d,%d %d", event.button.x, event.button.y, event.button.button);
                mx = event.button.x;
                my = event.button.y;
            }

        }
        // unsigned long start=get_time_usec();

        SDL_Rect window_rec;
        window_rec.w = WORLD_SIZE;
        window_rec.h = WORLD_SIZE;
        window_rec.x = 0;
        window_rec.y = 0;

        SDL_RenderCopy(renderer, map, NULL, &window_rec);
        if (mx < WORLD_SIZE && my < WORLD_SIZE)
        {
            Region * r = find_region(mx, my);
            char buf[256];
            for (int i=0; i < r->rocks_count; i++)
            {
                sprintf(buf, "%d %f", r->rocks_types[i]->terrain->id, r->rocks_types[i]->value);
                write_text(260, i*32, buf, White, 15, 30);
            }
            print_status(1, "rocks %ld", r->rocks_count);
        }

               //  unsigned long stop=get_time_usec();

               //print_status(1, "time %ld us", (stop-start));

        if (status_line[0] != ' ')
        {
            write_text(5, window_height - 64, status_line, White, 15, 30);
        }
        if (status_line2[0] != ' ')
        {
            write_text(5, window_height - 32, status_line2, White, 15, 30);
        }

        SDL_RenderPresent(renderer);
        //SDL_Delay(20);
    }
end:
    SDL_Quit();
    return 0;

    return 0;
}
