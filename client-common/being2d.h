#ifndef BEING_SDL_H
#define BEING_SDL_H

#include "../core/alchemist/elements.h"
#include "texture.h"
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
    float get_scale() override;
    bool check_rect(float px, float py, int t_size) override;
};

class Animal2d : public Animal, public Renderable
{
  public:
    Backend_Texture get_texture() override;
    bool going_right;
    Animal2d(Animal data);
    void render(int x, int y) override;
    void update_item_location(ItemLocation & old_loc, ItemLocation & new_loc) override;
    float get_scale() override;
    bool check_rect(float px, float py, int t_size) override;
};

#endif
