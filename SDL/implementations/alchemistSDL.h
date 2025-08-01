#ifndef ALCHEMIST_SDL_H
#define ALCHEMIST_SDL_H

#include "../../core/alchemist/elements.h"
#include "../networking.h"
#include "../texture.h"
#include "../window.h"

class Renderable
{
    SDL_Texture * texture;
    int w, h;

  protected:
    bool flip;

  public:
    Renderable()
    {
        flip = false;
        texture = nullptr;
        w = 0;
        h = 0;
    }
    virtual SDL_Texture * get_texture()
    {
        return NULL;
    }
    virtual float get_scale()
    {
        return 1.0;
    }
    virtual void render(SDL_Rect * rect)
    {
        if (!texture)
        {
            texture = get_texture();
            if (SDL_QueryTexture(texture, NULL, NULL, &w, &h) != 0)
            {
                printf("SDL_QueryTexture failed: %s", SDL_GetError());
            }
            else
            {
                // printf("%s: width: %d, heigh: %d\n", get_name(), width, height);
            }
        }
        float scale = get_scale();
        if (scale < 0.01)
            return;
        SDL_Rect img_rect = {rect->x, rect->y, (int)(w * scale), (int)(h * scale)};

        if (flip)
            SDL_RenderCopyEx(renderer, texture, NULL, &img_rect, 0, NULL, SDL_FLIP_HORIZONTAL);
        else
            SDL_RenderCopy(renderer, texture, NULL, &img_rect);
    }
};
// FIXME remove Renderable class
// core_class -> core_class_SDL { texture }

class IngredientSDL : public Ingredient, public Renderable
{
  public:
    SDL_Texture * get_texture();
    IngredientSDL(Ingredient data);
};

class ProductSDL : public Product, public Renderable
{
  public:
    SDL_Texture * get_texture();
    ProductSDL(Product data);
};

class ObjectSDL : public Object, public Renderable
{
  public:
    SDL_Texture * get_texture();
    ObjectSDL(int id);
};

class ElementSDL : public Element, public Renderable
{
    int start_width;
    SDL_Texture * el_texture;

  public:
    SDL_Texture * get_texture();

    ElementSDL(Element data);
    float get_scale() override
    {
        return 1.0 * width.value / start_width;
    }
    void show(bool details = true) override;
};

#endif
