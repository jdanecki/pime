#ifndef ALCHEMIST_SDL_H
#define ALCHEMIST_SDL_H

#include "../../core/alchemist/elements.h"
#include "../networking.h"
#include "../texture.h"
#include "../window.h"

class Renderable
{
  protected:
    bool flip;

  public:
    Renderable()
    {
        flip = false;
    }
    virtual SDL_Texture * get_texture()
    {
        return NULL;
    }
    virtual void render(SDL_Rect * rect)
    {
        if (flip)
            SDL_RenderCopyEx(renderer, get_texture(), NULL, rect, 0, NULL, SDL_FLIP_HORIZONTAL);
        else
            SDL_RenderCopy(renderer, get_texture(), NULL, rect);
    }
};

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
  public:
    SDL_Texture * get_texture();
    ElementSDL(Element data);
};

#endif
