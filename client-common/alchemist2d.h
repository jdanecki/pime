#ifndef ALCHEMIST_SDL_H
#define ALCHEMIST_SDL_H

#include "../core/alchemist/elements.h"
#include "backend.inl"

class Renderable
{
  protected:
    bool flip;
    Backend_Texture texture;
    bool texture_created;

  public:
    Renderable();
    virtual Backend_Texture get_texture();
    virtual void render(float el_x, float el_y, float w, float h);
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

class Field2d : public Field, public Renderable
{
  public:
    Backend_Texture get_texture();
    Field2d(Field data);
};

class Barn2d : public Barn, public Renderable
{
  public:
    Backend_Texture get_texture();
    Barn2d(Barn data);
};

class Element2d : public Element, public Renderable
{
  public:
    Element2d(Element data);
    void show(bool details = true) override;
};

class Scroll2d : public Scroll, public Renderable
{
  public:
    Scroll2d(Scroll data);
};

#endif
