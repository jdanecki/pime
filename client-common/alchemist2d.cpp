#include <math.h>

#include "alchemist2d.h"
#include "window.h"
#include "texture.h"

extern Backend_Texture * add_texture_color(Backend_Surface * s, ColorRGB c);

void hsv2rgb(int h, int s, int v, int * r, int * g, int * b);

Backend_Texture Place2d::get_texture()
{
    texture_created = true;
    return places_textures[id];
}

Place2d::Place2d(Place data) : Place(data)
{
}

Element2d::Element2d(Element data) : Element(data)
{
    w = width.value;
    h = height.value;
    start_width = width.value;
    /* value=0xff000000; //a
     value = 0x00ff0000; //b
     value = 0x0000ff00; //g
     value = 0x000000ff; //r
    */
    Backend_Pixels b_pixels = Backend_Allocate_Pixels(w, h);
    unsigned int * pixels = (unsigned int *)b_pixels.pixels;
    Form f = get_form();

    /* float vx = 1.0 + 1.0 * (rand() % 4);
     float vy = 1.0 + 1.0 * (rand() % 4);
     float vx1 = 1.0 + 1.0 * (rand() % 3);
     float vy1 = 1.0 + 1.0 * (rand() % 3);
 */
    float cx = width.value / 2.0f;
    float cy = height.value / 2.0f;

    float rx = width.value / 2.0f;
    float ry = height.value / 2.0f;
    int val = rand() % 100;

    float ang_ofs = 1.0 * (-15 + rand() % 25);
    for (unsigned int y = 0; y < height.value; y++)
    {
        for (unsigned int x = 0; x < width.value; x++)
        {
            float dx = 1.0 * (x - cx) / rx;
            float dy = 1.0 * (y - cy) / ry;
            float distance = dx * dx + dy * dy;
            float angle = atan2f(dy, dx);
            float offset;
            unsigned char base = 100 + rand() % 40;
            float inter = 0.1 * (rand() % 9);

            unsigned char r = (unsigned char)(get_base()->color.r * inter + base * (1.0 - inter));
            unsigned char g = (unsigned char)(get_base()->color.g * inter + base * (1.0 - inter));
            unsigned char b = (unsigned char)(get_base()->color.b * inter + base * (1.0 - inter));
            unsigned int c = (255 << 24) | (b << 16) | (g << 8) | r;

            switch (f)
            {
                case Form_solid:
                {
                    offset = 0.3f * sinf(angle * (6 + ang_ofs) + (float)(rand() % 100) / 50.0f);
                    pixels[y * width.value + x] = (distance <= 1.0f + offset) ? c : 0;
                    break;
                }
                case Form_liquid:
                    offset = 1.0f + 0.3f * sinf(angle * 6 + rand() % 100 / 50.0f);
                    // hsv2rgb(200 + (offset * 4), 100, 50 + val/2, &r, &g, &b);
                    b += offset * 4;
                    g += val / 2;
                    c = (255 << 24) | (b << 16) | (g << 8) | r;
                    pixels[y * width.value + x] = (distance <= offset) ? c : 0;
                    break;
                case Form_gas:
                {
                    float noiseAmount = 1.15f;
                    float deform = 1.0f + ((rand() % 1000) / 1000.0f - 0.5f) * noiseAmount;
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

                    pixels[y * width.value + x] = c;
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
    // CONSOLE_LOG("scale=%0.2f %d %d\n", get_scale(), width.value, start_width);
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
    w = 32;
    h = 32;
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
    w = 0;
    h = 0;
    texture_created = false;
}
Backend_Texture Renderable::get_texture()
{
    return texture;
}
float Renderable::get_scale()
{
    return 1.0;
}
void Renderable::render(int x, int y)
{
    if (!texture_created)
    {
        texture = get_texture();
        if (!Backend_Get_Texture_Size(texture, &w, &h))
        {
            //  CONSOLE_LOG("texture: width: %d, heigh: %d\n", w, h);
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
bool Renderable::check_rect(float px, float py, float x, float y, int t_size)
{
    float scale = get_scale();
    float size_x = w * scale;
    float size_y = h * scale;
    float rx = x + size_x;
    float by = y + size_y;

    if (scale < 0.01)
    {
        //    CONSOLE_LOG("px=%d  [x=%d..%d) py=%d [y=%d..%d) scale=%f\n", px, x, rx, py, y, by, scale);
        return false;
    }
    Backend_Rect el_rect((Backend_Rect_Field)x, (Backend_Rect_Field)y, (Backend_Rect_Field)(size_x), (Backend_Rect_Field)(size_y));
    Backend_Rect pl_rect((Backend_Rect_Field)px, (Backend_Rect_Field)py, (Backend_Rect_Field)t_size, (Backend_Rect_Field)t_size);
    /*    Backend_Rect frame=el_rect;
        frame.r.x-=left_top_world_x;
        frame.r.y-=left_top_world_y;
        Backend_Draw_Rectangle(frame, {255, 0, 255, 255});
        frame=pl_rect;
        frame.r.x-=left_top_world_x;
        frame.r.y-=left_top_world_y;
        Backend_Draw_Rectangle(frame, {255, 255, 0, 255});
    */
    bool ret = Backend_Has_Intersection(el_rect, pl_rect);
    /*    if (ret)
        {
            CONSOLE_LOG("el_rect: (%d,%d)-(%d,%d)\n", el_rect.r.x, el_rect.r.y, el_rect.r.x+el_rect.get_w(), el_rect.r.y+el_rect.get_h());
            CONSOLE_LOG("pl_rect: (%d,%d)-(%d,%d)\n", pl_rect.r.x, pl_rect.r.y, pl_rect.r.x+pl_rect.get_w(), pl_rect.r.y+pl_rect.get_h());
        }
    */
    return ret;
}
float Element2d::get_scale()
{
    return 1.0 * width.value / start_width;
}
bool Element2d::check_rect(float px, float py, int t_size)
{
    return Renderable::check_rect(px, py, location.get_world_x(), location.get_world_y(), t_size);
}
