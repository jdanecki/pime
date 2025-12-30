#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#include "../../../SDL/text.h"
#include "generator.h"

Region **regions;
Region * current_region;

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

SDL_Texture * map1, *map2, * map3;

void hsv2rgb(int h, int s, int v, int* r, int* g, int* b);

struct tile
{
    int tile;
    int form;
    int x;
    int y;
};

struct tile chunk[16];

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
                int r=0, g=0, b=0;
               // int v =reg->terrain_type->id *5;
               switch(reg->terrain_type->form)
               {
                   case 1: r=255; break; 
                   case 2: g=255; break;
                   case 3: b=255; break;
               }

               // hsv2rgb(60*(reg->terrain_type->form-1), 100 , 100, &r, &g, &b);

                pixels1[y * WORLD_SIZE + x] = 0xff000000 | (b << 16) | (g << 8) | r;

                hsv2rgb(18*reg->terrain_type->id, 100 , 100, &r, &g, &b);
                pixels2[y * WORLD_SIZE + x] = 0xff000000 | (b << 16) | (g << 8) | r;             
            }
        }
    }

    SDL_UnlockTexture(map1);
    SDL_UnlockTexture(map2);

}

void draw_map3()
{
    unsigned int * pixels3;
    int pitch, x, y;

    SDL_LockTexture(map3, NULL, (void **)&pixels3, &pitch);

    for (y = 0; y < CHUNK_SIZE; y++)
    {
        for (x = 0; x < CHUNK_SIZE; x++)
        {
            int r=0, g=0, b=0;
            int closest_tile=0;
            int min_dist = INT_MAX;

            for (int ch=0; ch < 16; ch++)
            {
                int dx = x - chunk[ch].x;
                int dy = y - chunk[ch].y;
                int dist=dx * dx + dy * dy;
                if (dist < min_dist)
                {
                    min_dist = dist;
                    closest_tile=ch;
                }
            }

            hsv2rgb(chunk[closest_tile].tile* 24,  100 , 100, &r, &g, &b);

            for (int ry = 0; ry < 16; ry++)
            {
                for (int rx = 0; rx < 16; rx++)
                {
                    pixels3[16*y * CHUNK_SIZE*16 + ry * CHUNK_SIZE*16 + 16*x+rx] = 0xff000000 | (b << 16) | (g << 8) | r;
                }
            }

        }
    }

    SDL_UnlockTexture(map3);
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
    SDL_SetWindowTitle(main_window, "generator");
    SDL_SetWindowPosition(main_window, 150, 10);
    SDL_SetWindowSize(main_window, 1024, 600 + 64 +256);
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

int random_bool(double probability)
{
    return ((double)rand() / RAND_MAX) < probability;
}

typedef void (*callback_fn)(int r);

void add_element(int r)
{
    int x = rand() % CHUNK_SIZE;
    int y = rand() % CHUNK_SIZE;

    chunk[r].tile = current_region->rocks_types[r]->terrain->id+1;
    chunk[r].form = current_region->rocks_types[r]->terrain->form;
    chunk[r].x=x;
    chunk[r].y=y;
}

void do_times(callback_fn f, int r)
{
#if 0
 int count = (int)(current_region->rocks_types[i]->value * 2.0f);
    for (int i = 0; i < count; ++i)
    {
        if (random_bool(0.5))
        {
            f(r->rocks_types[i]->terrain->id, , r->rocks_types[i]->terrain->form);
        }
    }
#else
    f(r);
#endif
}

int main() {
    srand((unsigned int)time(NULL));

    create_regions();

    /*for (int i = 0; i < 3; i++)
    {
        r[i]->show();
    }
*/

    printf("chunk=%ld\n", sizeof(chunk));
    init_window();
    map1 = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_ABGR8888, SDL_TEXTUREACCESS_STREAMING, WORLD_SIZE, WORLD_SIZE);
    map2 = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_ABGR8888, SDL_TEXTUREACCESS_STREAMING, WORLD_SIZE, WORLD_SIZE);
    map3 = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_ABGR8888, SDL_TEXTUREACCESS_STREAMING, CHUNK_SIZE*16, CHUNK_SIZE*16);
    draw_maps();

    char mode='m';
    bool redraw=true;

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
                redraw=true;
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

            if (mx < WORLD_SIZE && my < (3*WORLD_SIZE+20))
            {
                int ry= my < WORLD_SIZE ? my :
                             my < 2*WORLD_SIZE + 10 ? my-WORLD_SIZE-10 : my-2*WORLD_SIZE-20;
                if (ry >=0)
                {
                    current_region = find_region(mx, ry);
                    char buf[256];
                    write_text(260, 0, "rocks", Yellow, 15, 30);

                    if (redraw) memset(chunk, 0, sizeof(chunk));

                    //r->rocks_count < terrains_count=TILE_TEXTURES=15
                    for (int i=0; i < current_region->rocks_count; i++)
                    {
                        sprintf(buf, "id=%d form=%d r=%0.2f", current_region->rocks_types[i]->terrain->id,
                            current_region->rocks_types[i]->terrain->form, current_region->rocks_types[i]->value);
                        write_text(260, 32+i*32, buf, White, 15, 30);

                        //r->rocks_types[i]->terrain->id < TILE_TEXTURES=15
                        if (redraw) do_times(add_element, i);
                    }
                    if (redraw) {
                        draw_map3();
                        redraw=false;
                    }
                    write_text(600, 0, "plants", Yellow, 15, 30);
                    for (int i=0; i < current_region->plants_count; i++)
                    {
                        sprintf(buf, "id=%d grounds=%d r=%0.2f", current_region->plants_types[i]->plant->id,
                            current_region->plants_types[i]->plant->grounds_count,
                            current_region->plants_types[i]->value);
                        write_text(600, 32+i*32, buf, White, 15, 30);
                    }
                    print_status(1, "[%d,%d]=%p id=%d form=%d rocks=%ld plants=%ld size=%u", mx, ry,
                        current_region, current_region->terrain_type->id, current_region->terrain_type->form,
                        current_region->rocks_count, current_region->plants_count, current_region->size);
                } else status_line2[0]=0;
            }
            window_rec.y = 2*WORLD_SIZE+20;
            SDL_RenderCopy(renderer, map3, NULL, &window_rec);
        }
        if (mode == 'h') show_hsv_palette();

               //  unsigned long stop=get_time_usec();

               //print_status(1, "time %ld us", (stop-start));

        if (status_line[0])
        {
            write_text(5, window_height - 64, status_line, White, 15, 30);
        }
        if (status_line2[0])
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
