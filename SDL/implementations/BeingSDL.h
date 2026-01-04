#ifndef BEING_SDL_H
#define BEING_SDL_H

#include "../../core/alchemist/elements.h"
#include "../texture.h"
#include "alchemistSDL.h"
/*
class BeingSDL : public InventoryElement, public Renderable
{
  public:
    SDL_Texture * get_texture() override;

};
*/
class PlantSDL : public Plant, public Renderable
{    
  public:
    SDL_Texture * get_texture() override;
    PlantSDL(Plant data);
    float get_scale() override
    {
        return size;
    }
    bool check_rect(unsigned int px, unsigned int py, int t_size) override
    {
        return Renderable::check_rect(px, py, location.get_world_x(), location.get_world_y(), t_size);
    }

};

class AnimalSDL : public Animal, public Renderable
{
  public:
    SDL_Texture * get_texture() override;
    bool going_right;
    AnimalSDL(Animal data);
    void render(int x, int y) override;
    void update_item_location(ItemLocation & old_loc, ItemLocation & new_loc) override;
    float get_scale() override
    {
        return size;
    }
    bool check_rect(unsigned int px, unsigned int py, int t_size) override
    {
        return Renderable::check_rect(px, py, location.get_world_x(), location.get_world_y(), t_size);
    }
};

#endif
