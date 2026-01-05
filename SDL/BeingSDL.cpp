#include "BeingSDL.h"
#include "../core/networking.h"

/*SDL_Texture * BeingSDL::get_texture()
{
    return nullptr;
}
*/
SDL_Texture * AnimalSDL::get_texture()
{
    return animal_textures[get_id() % BASE_ANIMALS];
}

AnimalSDL::AnimalSDL(Animal data) : Animal(data)
{
}

void AnimalSDL::render(int x, int y)
{
    flip = !going_right;
    Renderable::render(x, y);
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
}
