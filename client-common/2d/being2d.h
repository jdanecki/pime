#ifndef BEING_SDL_H
#define BEING_SDL_H

#include "../core/alchemist/elements.h"
#include "alchemist2d.h"
/*
class BeingSDL : public InventoryElement, public Renderable
{
  public:
    Backend_Texture get_texture() override;

};
*/
class Plant2d : public Plant, public Renderable
{
  public:
    Backend_Texture get_texture() override;
    Plant2d(Plant data);
};

class Animal2d : public Animal, public Renderable
{
  public:
    Backend_Texture get_texture() override;
    bool going_right;
    Animal2d(Animal data);
    void update_item_location(ItemLocation & old_loc, ItemLocation & new_loc) override;
};

#endif
