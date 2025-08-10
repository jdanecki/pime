#include "alchemistSDL.h"

SDL_Texture * ObjectSDL::get_texture()
{ // FIXME
#if 0
    if (type == OBJECT_wall)
    {
        switch (base->id)
        {
            case ID_STONE:
                return object_textures[TEXTURE_stone_wall];
                break;
            case ID_LOG:
                return object_textures[TEXTURE_log_wall];
                break;
            case ID_LOG1:
                return object_textures[TEXTURE_log1_wall];
                break;
            case ID_LOG2:
                return object_textures[TEXTURE_log2_wall];
                break;
        }
    }
#endif
    return NULL;
}

ElementSDL::ElementSDL(Element data) : Element(data)
{
    w = width.value;
    h = height.value;

    texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_STREAMING, w, h);
    SDL_SetTextureBlendMode(texture, SDL_BLENDMODE_BLEND);

    start_width = width.value;
    /* value=0xff000000; //a
     value = 0xffff0000; //r
     value = 0xff00ff00; //g
     value = 0xff0000ff; //b
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
    float rx = 5.0 + (rand() % width.value) / 2;
    float ry = 5.0 + (rand() % height.value) / 2;

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

            unsigned char r2 = (unsigned char)(get_base()->color.r * inter + base * (1.0 - inter));
            unsigned char g2 = (unsigned char)(get_base()->color.g * inter + base * (1.0 - inter));
            unsigned char b2 = (unsigned char)(get_base()->color.b * inter + base * (1.0 - inter));
            c |= (r2 << 16) | (g2 << 8) | b2;

            switch (f)
            {
                case Form_solid:
                {
                    offset = 0.2f * sinf(angle * 5.0f + (float)(rand() % 100) / 50.0f);
                    pixels[y * width.value + x] = (distance <= 1.0f + offset) ? c : 0; // 0x40ffffff;
                    break;
                }
                case Form_liquid:
                    offset = 1.0f + 0.3f * sinf(angle * 6 + rand() % 100 / 50.0f);
                    c &= 0xff0f0fff;

                    pixels[y * width.value + x] = (distance <= offset) ? c : 0; // 0x40ffff00;
                    break;
                case Form_gas:
                    c |= 0xffff00 | get_base()->color.b;
                    ;
                    c = 0xffffffff;
                    pixels[y * width.value + x] = (distance <= 1.0f) ? c : 0;
                    break;
            }
        }
    }
    SDL_UnlockTexture(texture);
}

void ElementSDL::show(bool details)
{
    Element::show(details);
    printf("scale=%0.2f %d %d\n", get_scale(), width.value, start_width);
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

ScrollSDL::ScrollSDL(Scroll data):Scroll(data)
{
    w = 32;
    h = 32;

    texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_STREAMING, w, h);
    SDL_SetTextureBlendMode(texture, SDL_BLENDMODE_BLEND);
    unsigned int * pixels;
    int pitch;

    SDL_LockTexture(texture, NULL, (void **)&pixels, &pitch);
    for (int y = 0; y < h; ++y)
    {
        for (int x = 0; x < w; ++x)
        {
            if (x >= 2 && x < w - 2 && y >= 2 && y < w - 2)
            {
                pixels[y * w + x] = 0xFFDEB887;
            }
            else if (y < 4 || y >= w - 4)
            {
                pixels[y * w + x] = 0xFFA0522D;
            }
            else
            {
                pixels[y * w + x] = 0;
            }
        }
    }

    SDL_UnlockTexture(texture);
}
