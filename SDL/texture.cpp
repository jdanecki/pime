#include "texture.h"
#include "../core/alchemist/elements.h"
#include "../core/tiles.h"
#include "window.h"
#include <SDL2/SDL_blendmode.h>
#include <dirent.h>
#include <stdio.h>

struct textures Texture;
SDL_Texture * tiles_textures[TILE_MAX_NUM];
SDL_Texture * items_textures[BASE_ELEMENTS];
SDL_Texture * ing_textures[ING_ELEMENTS];
SDL_Texture * prod_textures[PROD_ELEMENTS];

SDL_Texture * animal_textures[BASE_ANIMALS];
SDL_Texture * plant_textures[BASE_PLANTS];

SDL_Texture * object_textures[TEXTURE_wall_max];
SDL_Texture * npc_texture;

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

void add_tile_texture(enum game_tiles id, const char * file)
{
    tiles_textures[id] = load_texture(file);
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
    struct dirent * entry;
    DIR * dir = opendir(dir_path);

    if (dir == NULL)
    {
        perror("Can't open textures directory'");
        exit(0);
    }

    while ((entry = readdir(dir)) != NULL)
    {
        char path[300];
        if (entry->d_name[0] == '.' && (entry->d_name[1] == '\0' || (entry->d_name[1] == '.' && entry->d_name[2] == '\0')))
        {
            continue;
        }
        sprintf(path, "%s/%s", dir_path, entry->d_name);
        printf("adding texture: %s\n", path);
        to[i++] = load_texture(path);
    }
    closedir(dir);
    return i;
}

void load_textures()
{
    Texture.player = load_texture("textures/player.png");

    Texture.run_icon = load_texture("textures/gui/run_icon.png");
    Texture.sneak_icon = load_texture("textures/gui/sneak_icon.png");

    npc_texture = load_texture("textures/npc.png");

    add_tile_texture(TILE_AIR, "textures/game_tiles/air.png");
    add_tile_texture(TILE_STONE, "textures/game_tiles/stone.png");
    add_tile_texture(TILE_DIRT, "textures/game_tiles/dirt.png");
    //    add_tile_texture(TILE_TREE, "textures/game_tiles/tree.png");
    add_tile_texture(TILE_SAND, "textures/game_tiles/sand.png");
    add_tile_texture(TILE_SANDSTONE, "textures/game_tiles/sandstone.png");
    //    add_tile_texture(TILE_SWEET_TREE, "textures/game_tiles/sweet_tree.png");
    //    add_tile_texture(TILE_SWEET_BUSH,"textures/game_tiles/sweet_bush.png");
    //    add_tile_texture(TILE_SWEET_FLOWER,"textures/game_tiles/sweet_flower.png");
    add_tile_texture(TILE_GRASS, "textures/game_tiles/grass.png");
    add_tile_texture(TILE_SWEET_GRASS, "textures/game_tiles/sweet_grass.png");
    add_tile_texture(TILE_WATER, "textures/game_tiles/water.png");

    int i = 0;
    i = add_textures_from_dir(items_textures, i, "textures/items/solid");
    i = add_textures_from_dir(items_textures, i, "textures/items/liquid");
    i = add_textures_from_dir(items_textures, i, "textures/items/gas");

    add_ing_texture(ING_AXE_BLADE, "textures/items/ingredients/axe_blade.png");
    add_ing_texture(ING_AXE_HANDLE, "textures/items/ingredients/axe_handle.png");
    add_ing_texture(ING_KNIFE_BLADE, "textures/items/ingredients/knife_blade.png");
    add_ing_texture(ING_KNIFE_HANDLE, "textures/items/ingredients/knife_handle.png");
    add_ing_texture(ING_PICKAXE_BLADE, "textures/items/ingredients/pickaxe_blade.png");
    add_ing_texture(ING_PICKAXE_HANDLE, "textures/items/ingredients/pickaxe_handle.png");
    add_ing_texture(ING_WALL, "textures/items/ingredients/wall.png");

    add_prod_texture(PROD_AXE, "textures/items/products/axe.png");
    add_prod_texture(PROD_KNIFE, "textures/items/products/knife.png");
    add_prod_texture(PROD_PICKAXE, "textures/items/products/pickaxe.png");
    add_prod_texture(PROD_HUT, "textures/objects/hut.png");

    i = 0;
    i = add_textures_from_dir(plant_textures, i, "textures/plants");

    animal_textures[0] = load_texture("textures/animals/pig.png");
    animal_textures[1] = load_texture("textures/animals/boar.png");
// FIXME
#if 0
    object_textures[TEXTURE_stone_wall] = load_texture("textures/objects/stone_wall.png");
    object_textures[TEXTURE_log_wall] = load_texture("textures/objects/log_wall.png");
    object_textures[TEXTURE_log1_wall] = load_texture("textures/objects/log1_wall.png");
    object_textures[TEXTURE_log2_wall] = load_texture("textures/objects/log2_wall.png");
#endif
}
