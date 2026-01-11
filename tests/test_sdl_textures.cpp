#include <limits.h>
#include <dirent.h>
#include <stdio.h>

#include  "../core/tiles.h"

#include "../client-common/window.h"
#include "../client-common/text.h"
#include "../client-common/texture.h"

int tile_size=32;
bool redraw=true;

struct tilexy
{
    int tile;
    int x;
    int y;
};

struct tilexy objects[TILE_TEXTURES];

chunk_table table;

SDL_Surface ** tiles_surfaces;
SDL_Texture * field;

bool key_pressed(int key)
{
    switch (key)
    {
        case SDLK_ESCAPE:
                return true;
        case SDLK_r:
                redraw=true;
                break;
    }
    return false;
}

void mouse_pressed(SDL_MouseButtonEvent & event)
{
    printf("mouse x: %d y: %d button=%d\n", event.x, event.y, event.button);
}


bool handle_events()
{
    SDL_Event event;

    while (SDL_PollEvent(&event))
    {
        if (event.type == SDL_QUIT)
        {
            SDL_Quit();
            return true;
        };
        if (event.type == SDL_KEYDOWN)
        {
            int key = event.key.keysym.sym;

            if (key_pressed(key))
                return true;
        }

        if (event.type == SDL_MOUSEBUTTONDOWN)
        {
            mouse_pressed(event.button);
        }
    }

    return 0;
}

unsigned int * get_pixel(SDL_Surface *surface, int x, int y)
{
    int bpp = surface->format->BytesPerPixel;
    unsigned char *p = (unsigned char *)surface->pixels + y * surface->pitch + x * bpp;
    return (unsigned int *)p;
}

SDL_Texture * texture_right;

void blend_pixels(unsigned int *pxA, unsigned int * pxB, SDL_PixelFormat * format)
{
    unsigned char rA, gA, bA, aA;
    unsigned char rB, gB, bB, aB;

    SDL_GetRGBA(*pxA, format, &rA, &gA, &bA, &aA);
    SDL_GetRGBA(*pxB, format, &rB, &gB, &bB, &aB);
    unsigned char noise=rand() % 40;
    unsigned char r = (rA + rB + noise) / 2 ;
    unsigned char g = (gA + gB + noise) / 2;
    unsigned char b = (bA + bB + noise) / 2;

    *pxA= SDL_MapRGBA(format, r, g, b, 255);
    //  *pxA= SDL_MapRGBA(format, 255, 255, 255, 255);
    *pxB= *pxA;
}

void prepare_texture()
{
    int s=tile_size*CHUNK_SIZE;

    SDL_Surface* surface = SDL_CreateRGBSurfaceWithFormat( 0, s, s, 32,SDL_PIXELFORMAT_RGBA8888);    

    memset(table, 0, sizeof(table));

    for (int ty = 0; ty < CHUNK_SIZE; ++ty)
    {
        for (int tx = 0; tx < CHUNK_SIZE; ++tx)
        {
            int closest_tile=0;
            int min_dist = INT_MAX;

            for (int t=0; t < TILE_TEXTURES; t++)
            {
                int dx = tx - objects[t].x;
                int dy = ty - objects[t].y;
                int dist=dx * dx + dy * dy;
                if (dist < min_dist)
                {
                    min_dist = dist;
                    closest_tile=t;
                }
            }
            table[ty][tx].tile=closest_tile;

            SDL_Surface * tile = tiles_surfaces[closest_tile];
            SDL_Rect img_rect = {tx * tile_size, ty * tile_size, tile_size, tile_size};
            SDL_Rect srcRect = {0, 0, tile_size, tile_size};
            SDL_BlitSurface(tile, &srcRect, surface, &img_rect);            
        }
    }

    for (int ty = 0; ty < CHUNK_SIZE; ++ty)
    {
        for (int tx = 0; tx < CHUNK_SIZE-1; ++tx)
        {
            if (table[ty][tx].tile!=table[ty][tx+1].tile)
            {
                for (int i = 0; i < tile_size; i++) {
                    for (int bw = 0; bw < 2; bw++)
                    {
                        unsigned int* pxA=get_pixel(surface, (tx)*tile_size+tile_size-1-bw, ty*tile_size+i);
                        unsigned int* pxB=get_pixel(surface, (tx+1)*tile_size+bw, ty*tile_size+i);

                        blend_pixels(pxA, pxB, surface->format);
                    }
                }
            }
            if (table[tx][ty].tile!=table[tx+1][ty].tile)
            {
                for (int i = 0; i < tile_size; i++) {
                    for (int bw = 0; bw < 2; bw++)
                    {
                        unsigned int* pxA=get_pixel(surface, ty*tile_size+i, tx*tile_size+tile_size-1-bw);
                        unsigned int* pxB=get_pixel(surface, ty*tile_size+i, (tx+1)*tile_size+bw);
                        blend_pixels(pxA, pxB, surface->format);
                    }
                }
            }
        }
    }

    if (texture_right) SDL_DestroyTexture(texture_right);
    texture_right = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_FreeSurface(surface);
}

void draw()
{
       for (int ty = 0; ty < CHUNK_SIZE; ++ty)
        {
            for (int tx = 0; tx < CHUNK_SIZE; ++tx)
            {
                int closest_tile=0;
                int min_dist = INT_MAX;

                for (int t=0; t < TILE_TEXTURES; t++)
                {
                    int dx = tx - objects[t].x;
                    int dy = ty - objects[t].y;
                    int dist=dx * dx + dy * dy;
                    if (dist < min_dist)
                    {
                        min_dist = dist;
                        closest_tile=t;
                    }
                }
                SDL_Texture * texture = tiles_textures[closest_tile];
                SDL_Rect img_rect = {tx * tile_size, ty * tile_size, tile_size, tile_size};
                SDL_RenderCopy(renderer, texture, NULL, &img_rect);
            }
        }
                    
       for (int t=0; t < TILE_TEXTURES; t++)
       {
            SDL_Texture * texture = tiles_textures[t];                
            SDL_Rect img_rect = {t * tile_size, 560, tile_size, tile_size};
            SDL_RenderCopy(renderer, texture, NULL, &img_rect);
            
            texture = SDL_CreateTextureFromSurface(renderer, tiles_surfaces[t]);
            img_rect = {600+t * tile_size, 560, tile_size, tile_size};
            SDL_RenderCopy(renderer, texture, NULL, &img_rect);
            SDL_DestroyTexture(texture);
       }      
}

int add_textures_from_dir(SDL_Texture ** to, SDL_Surface ** to_surf, int i, const char * dir_path)
{
    DIR * dir = opendir(dir_path);

    if (dir == NULL)
    {
        perror("Can't open textures directory'");
        exit(0);
    }
    closedir(dir);

    struct dirent ** namelist;
    int n = scandir(dir_path, &namelist, NULL, alphasort);
    if (n < 0)
        perror("scandir");
    else
    {
        char path[300];
        while (n--)
        {
            if (namelist[n]->d_name[0] == '.' && (namelist[n]->d_name[1] == '\0' || (namelist[n]->d_name[1] == '.' && namelist[n]->d_name[2] == '\0')))
            {
                continue;
            }
            sprintf(path, "%s/%s", dir_path, namelist[n]->d_name);
            printf("adding texture: %s\n", path);
        
            to_surf[i] = IMG_Load(path);
            if (!(to_surf[i]))
            {
                printf("Unable to load texture: %s error: %s\n", path, SDL_GetError());
                exit(0);
            }
            else
            {
                int bpp = to_surf[i]->format->BytesPerPixel;
                printf("bpp=%d\n", bpp);
                to[i] = SDL_CreateTextureFromSurface(renderer, to_surf[i]);

                if (!(to[i]))
                {
                    printf("Unable to create texture: %s error: %s\n", path, SDL_GetError());
                    exit(0);
                }
                i++;
            }
            free(namelist[n]);
        }
        free(namelist);
    }

    return i;
}

void load_field()
{
    SDL_Surface * surf = IMG_Load("textures/as_cornfield0/idle/135/field.png");
    field = SDL_CreateTextureFromSurface(renderer, surf);
    SDL_FreeSurface(surf);
}

int main()
{
    if (init_window("test textures", 1200,600))
        return 1;
    
    int i=0;
    tiles_textures = new SDL_Texture *[TILE_TEXTURES];
    tiles_surfaces = new SDL_Surface *[TILE_TEXTURES];
    tiles_textures_count = add_textures_from_dir(tiles_textures, tiles_surfaces, i, "textures/game_tiles");

    load_field();


    for (;;)
    {
        SDL_Rect img_rect = {600, 0, tile_size*CHUNK_SIZE, tile_size*CHUNK_SIZE};
        clear_window();

        if (handle_events())
            break;
        if (redraw) {
            memset(objects, 0, sizeof(objects));
            redraw=false;
            int tiles=1 + (rand() % (TILE_TEXTURES-1));
            for (int t=0; t < tiles; t++)
            {
                int x = rand() % CHUNK_SIZE;
                int y = rand() % CHUNK_SIZE;

                objects[t].x=x;
                objects[t].y=y;
            }
            prepare_texture();
        }
        draw();
        SDL_RenderCopy(renderer, texture_right, NULL, &img_rect);
        img_rect.x = 100;
        img_rect.y = 100;
        img_rect.w = tile_size;
        img_rect.h = tile_size;
        SDL_RenderCopy(renderer, field, NULL, &img_rect);

        SDL_RenderPresent(renderer);
        SDL_Delay(20);
    }
    return 0;
}
