#include "texture.h"
#include "../core/alchemist/elements.h"
#include "../core/tiles.h"
#include "window.h"
#include <SDL2/SDL_blendmode.h>
#include <SDL2/SDL_image.h>
#include <dirent.h>
#include <stdio.h>

struct player_textures Player_textures;

int tiles_textures_count;
SDL_Texture ** tiles_textures;

SDL_Texture * ing_textures[ING_COUNT];
SDL_Texture * prod_textures[PROD_COUNT];
SDL_Surface * scroll_surface;
SDL_Texture * animal_textures[BASE_ANIMALS];
SDL_Texture * plant_textures[BASE_PLANTS];

SDL_Texture * object_textures[TEXTURE_wall_max];

SDL_Texture * load_texture(const char * texture_name)
{
    SDL_Texture * texture = NULL;
    SDL_Surface * loadedSurface = IMG_Load(texture_name);
    if (loadedSurface == NULL)
    {
        printf("Unable to load texture: %s error: %s\n", texture_name, SDL_GetError());
        exit(0);
    }
    else
    {
        texture = SDL_CreateTextureFromSurface(renderer, loadedSurface);

        if (texture == NULL)
        {
            printf("Unable to create texture: %s error: %s\n", texture_name, SDL_GetError());
            exit(0);
        }
        SDL_FreeSurface(loadedSurface);
    }

    return texture;
}

void add_items_texture(int id, const char * file)
{
}

void add_ing_texture(int id, const char * file)
{
    printf("adding ing texture: %s\n", file);
    ing_textures[id] = load_texture(file);
}

void add_prod_texture(int id, const char * file)
{
    printf("adding product texture: %s\n", file);
    prod_textures[id] = load_texture(file);
}

int add_textures_from_dir(SDL_Texture ** to, int i, const char * dir_path)
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
            to[i++] = load_texture(path);
            free(namelist[n]);
        }
        free(namelist);
    }

    return i;
}

void load_textures()
{
    Player_textures.npc = IMG_Load("textures/player.png");
    // SDL_PIXELFORMAT_ABGR8888 for surface
    // printf("surface format=%x %s rgba=%x argb=%x\n", surface->format->format, SDL_GetPixelFormatName(surface->format->format), SDL_PIXELFORMAT_RGBA8888, SDL_PIXELFORMAT_ARGB8888);
    /*
    bajt 0: R
    bajt 1: G
    bajt 2: B
    bajt 3: A
Bits:   [31..24]   [23..16]   [15..8]   [7..0]
Field:     A          B          G        R
*/
    Player_textures.player = SDL_CreateTextureFromSurface(renderer, Player_textures.npc);

    Player_textures.run_icon = load_texture("textures/gui/run_icon.png");
    Player_textures.sneak_icon = load_texture("textures/gui/sneak_icon.png");

    int i = 0;

    // FIXME check number of tiles textures
    tiles_textures = new SDL_Texture *[20];

    tiles_textures_count = add_textures_from_dir(tiles_textures, i, "textures/game_tiles");

    add_ing_texture(ING_AXE_BLADE, "textures/items/ingredients/axe_blade.png");
    add_ing_texture(ING_AXE_HANDLE, "textures/items/ingredients/axe_handle.png");
    add_ing_texture(ING_KNIFE_BLADE, "textures/items/ingredients/knife_blade.png");
    add_ing_texture(ING_KNIFE_HANDLE, "textures/items/ingredients/knife_handle.png");
    add_ing_texture(ING_PICKAXE_BLADE, "textures/items/ingredients/pickaxe_blade.png");
    add_ing_texture(ING_PICKAXE_HANDLE, "textures/items/ingredients/pickaxe_handle.png");
    add_ing_texture(ING_WALL, "textures/items/ingredients/wall.png");
    add_ing_texture(ING_MEAT, "textures/items/ingredients/meat.png");
    add_ing_texture(ING_LOG, "textures/items/ingredients/log.png");
    add_ing_texture(ING_STICK, "textures/items/ingredients/stick.png");
    add_ing_texture(ING_TINDER, "textures/items/ingredients/tinder.png");
    add_ing_texture(ING_FRUIT, "textures/items/ingredients/fruit.png");

    add_prod_texture(PROD_AXE, "textures/items/products/axe.png");
    add_prod_texture(PROD_KNIFE, "textures/items/products/knife.png");
    add_prod_texture(PROD_PICKAXE, "textures/items/products/pickaxe.png");
    add_prod_texture(PROD_FIRE, "textures/items/products/fire.png");
    add_prod_texture(PROD_ROASTED_MEAT, "textures/items/products/roasted_meat.png");
    add_prod_texture(PROD_FRUIT_SALAD, "textures/items/products/fruit_salad.png");

    add_prod_texture(PROD_HUT, "textures/objects/hut.png");

    i = 0;
    i = add_textures_from_dir(plant_textures, i, "textures/plants");

    animal_textures[0] = load_texture("textures/animals/pig.png");
    animal_textures[1] = load_texture("textures/animals/boar.png");

    scroll_surface = IMG_Load("textures/scroll.png");

// FIXME
#if 0
    object_textures[TEXTURE_stone_wall] = load_texture("textures/objects/stone_wall.png");
    object_textures[TEXTURE_log_wall] = load_texture("textures/objects/log_wall.png");
    object_textures[TEXTURE_log1_wall] = load_texture("textures/objects/log1_wall.png");
    object_textures[TEXTURE_log2_wall] = load_texture("textures/objects/log2_wall.png");
#endif
}

SDL_Texture * add_texture_color(SDL_Surface * s, Color c)
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
