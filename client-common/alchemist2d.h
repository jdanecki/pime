#ifndef ALCHEMIST_SDL_H
#define ALCHEMIST_SDL_H

#include "../core/alchemist/elements.h"
#include "../core/player.h"

#include "net.h"
#include "texture.h"
#include "window.h"

class Renderable
{
  protected:
    int w, h;
    bool flip;
    Backend_Texture texture;
    bool texture_created;

  public:
    Renderable();
    virtual Backend_Texture get_texture();
    virtual float get_scale();
    virtual void render(int x, int y);
    bool check_rect(float px, float py, float x, float y, int t_size);
};

class Ingredient2d : public Ingredient, public Renderable
{
  public:
    Backend_Texture get_texture();
    Ingredient2d(Ingredient data);
};

class Product2d : public Product, public Renderable
{
  public:
    Backend_Texture get_texture();
    Product2d(Product data);
};

class Place2d : public Place, public Renderable
{
  public:
    Backend_Texture get_texture();
    Place2d(Place data);
};

class Element2d : public Element, public Renderable
{
    int start_width;

  public:
    Element2d(Element data);
    float get_scale() override;
    void show(bool details = true) override;
    bool check_rect(float px, float py, int t_size) override;
};

class Scroll2d : public Scroll, public Renderable
{
  public:
    Scroll2d(Scroll data);
};

#endif
