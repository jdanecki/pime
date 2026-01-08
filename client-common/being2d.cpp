#include "being2d.h"
#include "../core/networking.h"

/*Backend_Texture BeingSDL::get_texture()
{
    return nullptr;
}
*/
Backend_Texture Animal2d::get_texture()
{
    texture_created=true;
    return animal_textures[get_id() % BASE_ANIMALS];
}

Animal2d::Animal2d(Animal data) : Animal(data)
{
}

void Animal2d::render(int x, int y)
{
    flip = !going_right;
    Renderable::render(x, y);
}

void Animal2d::update_item_location(ItemLocation & old_loc, ItemLocation & new_loc)
{
    going_right = (old_loc.chunk.x < new_loc.chunk.x) ? true : false;
}

Backend_Texture Plant2d::get_texture()
{
    texture_created=true;
    // FIXME combine plant textures in one file for each phase
    // if (grown)
    return plant_textures[get_id() % BASE_PLANTS];
}

Plant2d::Plant2d(Plant data) : Plant(data)
{
}
