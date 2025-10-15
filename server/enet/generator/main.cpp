#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#include "../../../SDL/text.h"
#include "generator.h"

Region **regions;

SDL_Renderer * renderer;
SDL_Window * main_window;
int window_width;
int window_height;
int mx=0, my=0;
void show_hsv_palette();

unsigned long get_time_usec()
{
    struct timespec t;

    clock_gettime(CLOCK_MONOTONIC_RAW, &t);
    return (t.tv_sec * 1000000 + t.tv_nsec / 1000);
}

SDL_Texture * map1, *map2;

void hsv2rgb(int h, int s, int v, int* r, int* g, int* b);

void draw_maps()
{
    unsigned int * pixels1;
    unsigned int * pixels2;
    int pitch, x, y;

    SDL_LockTexture(map1, NULL, (void **)&pixels1, &pitch);
    SDL_LockTexture(map2, NULL, (void **)&pixels2, &pitch);

    for (y = 0; y < WORLD_SIZE; y++)
    {
        for (x = 0; x < WORLD_SIZE; x++)
        {
            Region * reg = find_region(x, y);
            {
                int r, g, b;
                int v =reg->terrain_type->id *5;
                hsv2rgb(60*(reg->terrain_type->form-1), 100 , 100, &r, &g, &b);
                pixels1[y * WORLD_SIZE + x] = 0xff000000 | (b << 16) | (g << 8) | r;
                hsv2rgb(18*reg->terrain_type->id, 100 , 100, &r, &g, &b);
                pixels2[y * WORLD_SIZE + x] = 0xff000000 | (b << 16) | (g << 8) | r;
            }
        }
    }

    SDL_UnlockTexture(map1);
    SDL_UnlockTexture(map2);
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
    SDL_SetWindowSize(main_window, 1024, 600 + 64);
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

void show_terrains()
{
    for (int i=0; i < terrains_count; i++)
    {
        terrains[i]->show();
    }
}

void show_plants()
{
    for (int i=0; i < all_plants_count; i++)
    {
        all_plants[i]->show();
    }
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
    map1 = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_ABGR8888, SDL_TEXTUREACCESS_STREAMING, WORLD_SIZE, WORLD_SIZE);
    map2 = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_ABGR8888, SDL_TEXTUREACCESS_STREAMING, WORLD_SIZE, WORLD_SIZE);
    draw_maps();

    char mode='m';

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
                        mode='m';
                        break;
                    }
                    case 't': show_terrains(); break;
                    case 'p': show_plants(); break;
                    case 'h': mode='h'; break;
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

        if (mode == 'm')
        {
            SDL_Rect window_rec;
            window_rec.w = WORLD_SIZE;
            window_rec.h = WORLD_SIZE;
            window_rec.x = 0;
            window_rec.y = 0;

            SDL_RenderCopy(renderer, map1, NULL, &window_rec);
            window_rec.y = WORLD_SIZE+10;
            SDL_RenderCopy(renderer, map2, NULL, &window_rec);
            if (mx < WORLD_SIZE && my < (2*WORLD_SIZE+10))
            {
                int ry= my < WORLD_SIZE ? my : my-WORLD_SIZE-10;
                if (ry >=0)
                {
                    Region * r = find_region(mx, ry);
                    char buf[256];
                    write_text(260, 0, "rocks", Yellow, 15, 30);
                    for (int i=0; i < r->rocks_count; i++)
                    {
                        sprintf(buf, "id=%d form=%d r=%0.2f", r->rocks_types[i]->terrain->id, r->rocks_types[i]->terrain->form, r->rocks_types[i]->value);
                        write_text(260, 32+i*32, buf, White, 15, 30);
                    }
                    write_text(600, 0, "plants", Yellow, 15, 30);
                    for (int i=0; i < r->plants_count; i++)
                    {
                        sprintf(buf, "id=%d grounds=%d r=%0.2f", r->plants_types[i]->plant->id, r->plants_types[i]->plant->grounds_count,r->plants_types[i]->value);
                        write_text(600, 32+i*32, buf, White, 15, 30);
                    }
                    print_status(1, "[%d,%d]=%p id=%d form=%d rocks=%ld plants=%ld size=%u", mx, ry, r, r->terrain_type->id, r->terrain_type->form,
                        r->rocks_count, r->plants_count,r->size);
                } else status_line2[0]=0;
            }
        }
        if (mode == 'h') show_hsv_palette();

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
