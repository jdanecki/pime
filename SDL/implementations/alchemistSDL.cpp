#include "alchemistSDL.h"

void hsv2rgb(int h, int s, int v, int* r, int* g, int* b);

SDL_Texture * PlaceSDL::get_texture()
{
    return places_textures[id];
}

PlaceSDL::PlaceSDL(Place data) : Place(data)
{

}

ElementSDL::ElementSDL(Element data) : Element(data)
{
    w = width.value;
    h = height.value;

    texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_ABGR8888, SDL_TEXTUREACCESS_STREAMING, w, h);
    SDL_SetTextureBlendMode(texture, SDL_BLENDMODE_BLEND);

    start_width = width.value;
    /* value=0xff000000; //a
     value = 0xffff0000; //b
     value = 0xff00ff00; //g
     value = 0xff0000ff; //r
    */
    unsigned int * pixels;
    int pitch;

    SDL_LockTexture(texture, NULL, (void **)&pixels, &pitch);

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
            unsigned int c = 255 << 24;
            unsigned char base = 100 + rand() % 40;
            float inter = 0.1 * (rand() % 9);

            unsigned char r = (unsigned char)(get_base()->color.r * inter + base * (1.0 - inter));
            unsigned char g = (unsigned char)(get_base()->color.g * inter + base * (1.0 - inter));
            unsigned char b = (unsigned char)(get_base()->color.b * inter + base * (1.0 - inter));
            c |= (b << 16) | (g << 8) | r;

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
                        c = (((int)(255.0*edgeFade1)) <<24) | (b << 16) | (g << 8) | r;
                    } else c =0;

                    pixels[y * width.value + x] = c;
                    break;
                }
            }
        }
    }
    SDL_UnlockTexture(texture);
}

void ElementSDL::show(bool details)
{
    Element::show(details);
    // CONSOLE_LOG("scale=%0.2f %d %d\n", get_scale(), width.value, start_width);
}

IngredientSDL::IngredientSDL(Ingredient data) : Ingredient(data)
{
}

SDL_Texture * IngredientSDL::get_texture()
{
    return ing_textures[get_id()];
}

ProductSDL::ProductSDL(Product data) : Product(data)
{
}

SDL_Texture * ProductSDL::get_texture()
{
    return prod_textures[get_id()];
}

ScrollSDL::ScrollSDL(Scroll data) : Scroll(data)
{
    w = 32;
    h = 32;
    Base * b = get_base();
    Color c;

    switch (b->c_id)
    {
        case Class_BaseElement:
        {
            BaseElement * el = get_base_element(b->id);
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

    texture = add_texture_color(scroll_surface, c);
}
