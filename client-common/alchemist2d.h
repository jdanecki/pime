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
    Renderable()
    {
        flip = false;
        w = 0;
        h = 0;
        texture_created = false;
    }
    virtual Backend_Texture get_texture()
    {
        return texture;
    }
    virtual float get_scale()
    {
        return 1.0;
    }
    virtual void render(int x, int y)
    {
        if (!texture_created)
        {
            texture = get_texture();
            if (!Backend_Get_Texture_Size(texture, &w, &h))
            {
                // CONSOLE_LOG("%s: width: %d, heigh: %d\n", get_name(), width, height);
            }
        }
        float scale = get_scale();
        if (scale < 0.01)
            return;

        Backend_Rect img_rect((Backend_Rect_Field)x, (Backend_Rect_Field)y, (Backend_Rect_Field)(w * scale), (Backend_Rect_Field)(h * scale));
        Backend_Rect_Field ww = window_width - PANEL_WINDOW;
        Backend_Rect_Field wh = window_height - STATUS_LINES;
        Backend_Rect src_rect(0, 0, w, h);

        if (x >= 0)
        {
            if (img_rect.r.x + img_rect.get_w() > ww)
            {
                img_rect.set_w(ww - img_rect.r.x);
                src_rect.set_w(img_rect.get_w());
            }
        }
        else
        {
            if (img_rect.r.x + img_rect.get_w() >= 0)
            {
                src_rect.r.x = -x;
                img_rect.add_w(img_rect.r.x);
                src_rect.set_w(img_rect.get_w());
                img_rect.r.x = 0;
            }
        }
        if (y >= 0)
        {
            if (img_rect.r.y + img_rect.get_h() > wh)
            {
                img_rect.set_h(wh - img_rect.r.y);
                src_rect.set_h(img_rect.get_h());
            }
        }
        else
        {
            if (img_rect.r.y + img_rect.get_h() >= 0)
            {
                src_rect.r.y = -y;
                img_rect.add_h(img_rect.r.y);
                src_rect.set_h(img_rect.get_h());
                img_rect.r.y = 0;
            }
        }
        if (flip)
            Backend_Texture_Copy_Flip(texture, &src_rect, &img_rect);
        else
            Backend_Texture_Copy_With_Mask(texture, &src_rect, &img_rect, {0}, false);
    }
    bool check_rect(unsigned int px, unsigned int py, unsigned int x, unsigned int y, int t_size)
    {
        float scale = get_scale();
        unsigned int rx = x + ((unsigned int)(w * scale) / t_size);
        unsigned int by = y + ((unsigned int)(h * scale) / t_size);

        if (scale < 0.01)
        {
            //    CONSOLE_LOG("px=%d  [x=%d..%d) py=%d [y=%d..%d) scale=%f\n", px, x, rx, py, y, by, scale);
            return false;
        }
        bool ret = ((px >= x) && (px <= rx) && (py >= y) && (py <= by));
        //   printf("px=%d  [x=%d..%d) py=%d [y=%d..%d) scale=%f ret=%d\n", px, x, rx, py, y, by, scale, ret);
        return ret;
    }
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
    float get_scale() override
    {
        return 1.0 * width.value / start_width;
    }
    void show(bool details = true) override;
    bool check_rect(unsigned int px, unsigned int py, int t_size) override
    {
        return Renderable::check_rect(px, py, location.get_world_x(), location.get_world_y(), t_size);
    }
};

class Scroll2d : public Scroll, public Renderable
{
  public:
    Scroll2d(Scroll data);
};

#endif
