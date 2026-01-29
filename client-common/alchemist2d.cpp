#include <cstdlib>
#include <math.h>

#include "../core/alchemist/random_functions.h"
#include "alchemist2d.h"
#include "backend.inl"
#include "window.h"
#include "texture.h"
#include "../dialog/d_hotbar.h"

int tile_size;

extern Backend_Texture * add_texture_color(Backend_Surface * s, ColorRGB c);

void hsv2rgb(int h, int s, int v, int * r, int * g, int * b);

Backend_Texture Field2d::get_texture()
{
    texture_created = true;
    return places_textures[id];
}

Field2d::Field2d(Field data) : Field(data)
{
}

Backend_Texture Barn2d::get_texture()
{
    texture_created = true;
    return places_textures[id];
}

Barn2d::Barn2d(Barn data) : Barn(data)
{
}
Element2d::Element2d(Element data) : Element(data)
{
    int w = dimensions.width.value * tile_size;
    int h = dimensions.height.value * tile_size;
    /* value=0xff000000; //a
     value = 0x00ff0000; //b
     value = 0x0000ff00; //g
     value = 0x000000ff; //r
    */
    Backend_Pixels b_pixels = Backend_Allocate_Pixels(w, h);
    unsigned int * pixels = (unsigned int *)b_pixels.pixels;
    Form f = get_form();

    float cx = w / 2.0f;
    float cy = h / 2.0f;

    float rx = w / 2.0f;
    float ry = h / 2.0f;
    int val = random_range(0, 100);

    float ang_ofs = random_float_range(-15, 40);
    for (unsigned int y = 0; y < h; y++)
    {
        for (unsigned int x = 0; x < w; x++)
        {
            float dx = 1.0 * (x - cx) / rx;
            float dy = 1.0 * (y - cy) / ry;
            float distance = dx * dx + dy * dy;
            float angle = atan2f(dy, dx);
            float offset;
            unsigned char base = random_range(100, 140);
            float inter = random_float_range(0.1, 0.9);

            unsigned char r = (unsigned char)(get_base()->color.r * inter + base * (1.0 - inter));
            unsigned char g = (unsigned char)(get_base()->color.g * inter + base * (1.0 - inter));
            unsigned char b = (unsigned char)(get_base()->color.b * inter + base * (1.0 - inter));
            unsigned int c = (255 << 24) | (b << 16) | (g << 8) | r;

            switch (f)
            {
                case Form_solid:
                {
                    offset = 0.3f * sinf(angle * (6 + ang_ofs) + random_float_range(0, 2));
                    pixels[y * w + x] = (distance <= 1.0f + offset) ? c : 0;
                    break;
                }
                case Form_liquid:
                    offset = 1.0f + 0.3f * sinf(angle * 6 + random_float_range(0, 2));
                    // hsv2rgb(200 + (offset * 4), 100, 50 + val/2, &r, &g, &b);
                    b += offset * 4;
                    g += val / 2;
                    c = (255 << 24) | (b << 16) | (g << 8) | r;
                    pixels[y * w + x] = (distance <= offset) ? c : 0;
                    break;
                case Form_gas:
                {
                    float noiseAmount = 1.15f;
                    float deform = 1.0f + random_float_range(-0.5, 0.5) * noiseAmount;
                    if (distance <= deform)
                    {
                        float edgeFade1 = 1.0f - sqrtf(distance / deform);
                        float edgeFade2 = powf(edgeFade1, 1.5f);
                        r += 10.0 * edgeFade2;
                        g += 10.0 * edgeFade2;
                        b += 10.0 * edgeFade2;
                        c = (((unsigned char)(255.0 * edgeFade1)) << 24) | (b << 16) | (g << 8) | r;
                    }
                    else
                        c = 0;

                    pixels[y * w + x] = c;
                    break;
                }
            }
        }
    }
    Backend_Update_Texture_Pixels(b_pixels);
    texture = b_pixels.texture;
}

void Element2d::show(bool details)
{
    Element::show(details);
    // CONSOLE_LOG("%f\n",  width.value);
}

Ingredient2d::Ingredient2d(Ingredient data) : Ingredient(data)
{
}

Backend_Texture Ingredient2d::get_texture()
{
    texture_created = true;
    return ing_textures[get_id()];
}

Product2d::Product2d(Product data) : Product(data)
{
}

Backend_Texture Product2d::get_texture()
{
    texture_created = true;
    return prod_textures[get_id()];
}

Scroll2d::Scroll2d(Scroll data) : Scroll(data)
{
    Base * b = get_base();
    ColorRGB c;

    switch (b->c_id)
    {
        case Class_BaseElement:
        {
            BaseElement * el = get_base_element(b->uid);
            c = el->color;
            break;
        }
        case Class_BaseAnimal:
            c.r = 186;
            c.g = 89;
            c.b = 89;
            break;
        case Class_BasePlant:
            c.r = 0;
            c.g = 128;
            c.b = 0;
            break;
    }

    //    texture = add_texture_color(scroll_surface, c);
    texture_created = false;
}
Renderable::Renderable()
{
    flip = false;
    texture_created = false;
}
Backend_Texture Renderable::get_texture()
{
    return texture;
}

void Renderable::render(float el_x, float el_y, float w, float h)
{
    int x = el_x * tile_size;
    int y = el_y * tile_size;
    int pix_w = w * tile_size;
    int pix_h = h * tile_size;
    if (!texture_created)
    {
        texture = get_texture();
    }

    Backend_Rect img_rect((Backend_Rect_Field)x, (Backend_Rect_Field)(HOTBAR_HEIGHT + y), (Backend_Rect_Field)(pix_w), (Backend_Rect_Field)(pix_h));
    if (img_rect.get_h() > window_height - STATUS_LINES - HOTBAR_HEIGHT)
    {
        return;
    }
    if (flip)
    {
        Backend_Texture_Copy_Flip(texture, nullptr, &img_rect);
        /*   Backend_Line(x, y, x + pix_w, y + pix_h, {255, 255, 255, 255});
           Backend_Line(x, y + pix_h, x + pix_w, y, {255, 255, 255, 255});
           Backend_Draw_Rectangle(img_rect, {255, 0, 0, 255});*/
    }
    else
    {
        Backend_Texture_Copy_With_Mask(texture, nullptr, &img_rect, {0}, false);
        /*Backend_Line(x, y, x + pix_w, y + pix_h, {255, 255, 255, 255});
        Backend_Line(x, y + pix_h, x + pix_w, y, {255, 255, 255, 255});
        Backend_Draw_Rectangle(img_rect, {255, 0, 0, 255});*/
    }
}
