#ifndef ALCHEMIST_SDL_H
#define ALCHEMIST_SDL_H

#include "../core/alchemist/elements.h"
#include "../core/player.h"
#include "../core/networking.h"
#include "texture.h"
#include "window.h"
#include <SDL_render.h>

class Renderable
{    
  protected:
    int w, h;
    bool flip;
    SDL_Texture * texture;

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
        return texture;
    }
    virtual float get_scale()
    {
        return 1.0;
    }
    virtual void render(int x, int y)
    {
        if (!texture)
        {
            texture = get_texture();
            if (SDL_QueryTexture(texture, NULL, NULL, &w, &h) != 0)
            {
                CONSOLE_LOG("SDL_QueryTexture failed: %s", SDL_GetError());
            }
            else
            {
                // CONSOLE_LOG("%s: width: %d, heigh: %d\n", get_name(), width, height);
            }
        }
        float scale = get_scale();
        if (scale < 0.01)
            return;

        SDL_Rect img_rect = {x, y, (int)(w * scale), (int)(h * scale)};
        int ww = window_width - PANEL_WINDOW;
        int wh = window_height - STATUS_LINES;
        SDL_Rect src_rect={0, 0 , w, h};

        if (x >=0 )
        {
            if (img_rect.x + img_rect.w > ww)
            {
                img_rect.w = ww - img_rect.x;
                src_rect.w  = img_rect.w;
            }
        } else {
            if (img_rect.x + img_rect.w >=0)
            {
                src_rect.x = -x;                
                img_rect.w += img_rect.x;                
                src_rect.w  = img_rect.w;
                img_rect.x = 0;
            }
        }
        if (y >= 0 )
        {
            if (img_rect.y + img_rect.h > wh)
            {
                img_rect.h = wh - img_rect.y;
                src_rect.h  = img_rect.h;
            }
        } else {
            if (img_rect.y + img_rect.h >=0)
            {
                src_rect.y = -y;                
                img_rect.h += img_rect.y;                
                src_rect.h  = img_rect.h;
                img_rect.y = 0;
            }
        }

        if (flip)
            SDL_RenderCopyEx(renderer, texture, &src_rect, &img_rect, 0, NULL, SDL_FLIP_HORIZONTAL);
        else
            SDL_RenderCopy(renderer, texture, &src_rect, &img_rect);
     //   SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
     //   SDL_RenderDrawRect(renderer, &img_rect);
    }
    bool check_rect(unsigned int px, unsigned int py, unsigned int x, unsigned int y, int t_size)
    {
        float scale = get_scale();
        unsigned int rx = x + ((unsigned int)(w * scale) / t_size);
        unsigned int by = y + ((unsigned int)(h * scale) / t_size);

        if (scale < 0.01) {
        //    CONSOLE_LOG("px=%d  [x=%d..%d) py=%d [y=%d..%d) scale=%f\n", px, x, rx, py, y, by, scale);
            return false;
        }
        bool ret= (
            (px >= x) &&
            (px <= rx) &&
            (py >= y) &&
            (py <= by));
     //   printf("px=%d  [x=%d..%d) py=%d [y=%d..%d) scale=%f ret=%d\n", px, x, rx, py, y, by, scale, ret);
        return ret;
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

class PlaceSDL : public Place, public Renderable
{
  public:
    SDL_Texture * get_texture();
    PlaceSDL(Place data);
};

class ElementSDL : public Element, public Renderable
{
    int start_width;

  public:
    ElementSDL(Element data);
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

class ScrollSDL : public Scroll, public Renderable
{
  public:
    ScrollSDL(Scroll data);
};

#endif
