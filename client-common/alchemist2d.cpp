#include <math.h>

#include "alchemist2d.h"

extern Backend_Texture* add_texture_color(Backend_Surface* s, ColorRGB c);

void hsv2rgb(int h, int s, int v, int* r, int* g, int* b);

Backend_Texture Place2d::get_texture()
{
    texture_created=true;
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
    unsigned int * pixels=(unsigned int *) b_pixels.pixels;
    Form f = get_form();

    /* float vx = 1.0 + 1.0 * (rand() % 4);
     float vy = 1.0 + 1.0 * (rand() % 4);
     float vx1 = 1.0 + 1.0 * (rand() % 3);
     float vy1 = 1.0 + 1.0 * (rand() % 3);
 */
    float cx = width.value / 2.0f;
    float cy = height.value / 2.0f;
    
    float rx = width.value / 2;
    float ry = height.value / 2;
    int val=rand() % 100;

    float ang_ofs= 1.0 * (-15 + rand() % 25);    
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
            unsigned int c = (255 << 24) | (b << 16) | (g <<8) | r;

            switch (f)
            {
                case Form_solid:
                {
                    offset = 0.3f * sinf(angle * (6+ang_ofs) +  (float)(rand() % 100) / 50.0f);
                    pixels[y * width.value + x] = (distance <= 1.0f + offset) ? c : 0;
                    break;
                }
                case Form_liquid:
                    offset = 1.0f + 0.3f * sinf(angle * 6 + rand() % 100 / 50.0f);
                    //hsv2rgb(200 + (offset * 4), 100, 50 + val/2, &r, &g, &b);
                    b+= offset*4;
                    g+=val/2;
                    c = (255<<24) | (b << 16) | (g << 8) | r;
                    pixels[y * width.value + x] = (distance <= offset) ? c : 0;
                    break;
                case Form_gas:
                {
                    float noiseAmount = 1.15f;
                    float deform = 1.0f + ((rand() % 1000) / 1000.0f - 0.5f) * noiseAmount;
                    if (distance <= deform) {
                        float edgeFade1 = 1.0f - sqrtf(distance / deform);
                        float edgeFade2 = powf(edgeFade1, 1.5f);
                        r+=10.0*edgeFade2;
                        g+=10.0*edgeFade2;
                        b+=10.0*edgeFade2;
                        c = ( ((unsigned char)(255.0*edgeFade1)) << 24) | (b << 16) | (g << 8) | r;
                    } else c =0;

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
    texture_created=true;
    return ing_textures[get_id()];
}

Product2d::Product2d(Product data) : Product(data)
{
}

Backend_Texture Product2d::get_texture()
{
    texture_created=true;
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
