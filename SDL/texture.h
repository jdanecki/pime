#ifndef TEXTURE_H
#define TEXTURE_H

#include <SDL2/SDL_surface.h>
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

struct player_textures
{
    SDL_Texture * player;
    SDL_Surface * npc;
    SDL_Texture * run_icon;
    SDL_Texture * sneak_icon;
};

extern int tiles_textures_count;
extern SDL_Texture ** tiles_textures;

extern SDL_Texture * ing_textures[ING_COUNT];
extern SDL_Texture * prod_textures[PROD_COUNT];
extern SDL_Texture * animal_textures[BASE_ANIMALS];
extern SDL_Texture * plant_textures[BASE_PLANTS];
extern SDL_Surface * scroll_surface;

extern SDL_Texture * object_textures[TEXTURE_wall_max];
extern SDL_Texture * up_mask;
extern SDL_Texture * down_mask;
extern struct player_textures Player_textures;

void load_textures();
SDL_Texture * load_texture(const char * texture_name);
SDL_Texture * add_texture_color(SDL_Surface * s, Color c);

#endif
