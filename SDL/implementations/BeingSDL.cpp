#include "BeingSDL.h"
#include "../networking.h"

SDL_Texture * BeingSDL::get_texture()
{
    return nullptr;
}

SDL_Texture * AnimalSDL::get_texture()
{
    return animal_textures[get_id() % 2];
}

AnimalSDL::AnimalSDL(Animal data) : Animal(data)
{
    // age->value = data->age;
    // max_age->value = data->max_age;
}

void AnimalSDL::update_item_location(ItemLocation & old_loc, ItemLocation & new_loc)
{
    going_right = (old_loc.chunk.x < new_loc.chunk.x) ? true : false;
}

SDL_Texture * PlantSDL::get_texture()
{
    // FIXME combine plant textures in one file for each phase
    // if (grown)
    return plant_textures[get_id() % BASE_PLANTS];
}

PlantSDL::PlantSDL(Plant data) : Plant(data)
{
    // grown = data->grown;
    // age->value = data->age;
    // max_age->value = data->max_age;
}
