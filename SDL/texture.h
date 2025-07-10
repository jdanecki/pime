#ifndef TEXTURE_H
#define TEXTURE_H

#include <stdlib.h>

#include <SDL2/SDL_image.h>

#include "../core/alchemist/elements.h"
#include "../core/tiles.h"

enum object_textures_enum
{
    TEXTURE_stone_wall,
    TEXTURE_log_wall,
    TEXTURE_log1_wall,
    TEXTURE_log2_wall,
    TEXTURE_wall_max
};

struct textures
{
    SDL_Texture * player;
    SDL_Texture * run_icon;
    SDL_Texture * sneak_icon;
};

extern SDL_Texture * tiles_textures[TILE_MAX_NUM];
extern SDL_Texture * items_textures[BASE_ELEMENTS];
extern SDL_Texture * ing_textures[ING_ELEMENTS];
extern SDL_Texture * prod_textures[PROD_ELEMENTS];
extern SDL_Texture * animal_textures[BASE_ANIMALS];
extern SDL_Texture * plant_textures[BASE_PLANTS];

extern SDL_Texture * object_textures[TEXTURE_wall_max];
extern SDL_Texture * npc_texture;
extern SDL_Texture * up_mask;
extern SDL_Texture * down_mask;
extern struct textures Texture;

void load_textures();
SDL_Texture * load_texture(const char * texture_name);

#endif
