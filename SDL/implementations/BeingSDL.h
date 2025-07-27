#ifndef BEING_SDL_H
#define BEING_SDL_H

#include "../../core/alchemist/elements.h"
#include "../texture.h"
#include "alchemistSDL.h"

class BeingSDL : public InventoryElement, public Renderable
{
  public:
    SDL_Texture * get_texture() override;
};

class PlantSDL : public Plant, public Renderable
{
    int * padding1; // FIXME
  public:
    SDL_Texture * get_texture() override;
    PlantSDL(Plant data);

    void render(SDL_Rect * rect) override
    {
        int width, height;

        if (SDL_QueryTexture(get_texture(), NULL, NULL, &width, &height) != 0)
        {
            printf("SDL_QueryTexture failed: %s", SDL_GetError());
        }
        else
        {
            // printf("%s: Szerokość: %d, Wysokość: %d\n", get_name(), width, height);
        }
        SDL_Rect img_rect = {rect->x, rect->y, width, height};
        Renderable::render(&img_rect);
    }
};

class AnimalSDL : public Animal, public Renderable
{
  public:
    SDL_Texture * get_texture() override;
    bool going_right;
    AnimalSDL(Animal data);
    void render(SDL_Rect * rect) override
    {
        flip = !going_right;
        Renderable::render(rect);
    }
    void update_item_location(ItemLocation & old_loc, ItemLocation & new_loc);
};

#endif
