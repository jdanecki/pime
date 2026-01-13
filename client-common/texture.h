#ifndef TEXTURE_H
#define TEXTURE_H

#include "../core/alchemist/elements.h"
#include "../core/tiles.h"
#include "backend.inl"

struct player_textures
{
    Backend_Texture player;
    Backend_Surface npc;
    Backend_Texture run_icon;
    Backend_Texture sneak_icon;
};

extern int tiles_textures_count;
extern Backend_Texture * tiles_textures;
extern Backend_Texture ing_textures[ING_COUNT];
extern Backend_Texture prod_textures[PROD_COUNT];
extern int base_animals_count;
extern Backend_Texture animal_textures[BASE_ANIMALS];
extern int base_plants_count;
extern Backend_Texture plant_textures[BASE_PLANTS];
extern Backend_Surface scroll_surface;
extern Backend_Texture places_textures[PLACES_COUNT];
extern Backend_Texture up_mask;
extern Backend_Texture down_mask;
extern struct player_textures Player_textures;

void load_textures();
Backend_Texture load_texture(const char * texture_name);

#endif
