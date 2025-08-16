#include "../core/alchemist/elements.h"

extern "C"
{
    struct SDL_Texture
    {
        int w;
        int h;
        void * pixels;
    };
    struct SDL_Surface
    {
        int w;
        int h;
    };
    struct SDL_Renderer
    {
    };

    SDL_Renderer * renderer;

    typedef struct SDL_Texture SDL_Texture;

    SDL_Texture * ing_textures[ING_COUNT];
    SDL_Texture * prod_textures[PROD_COUNT];
    SDL_Surface * scroll_surface;
    SDL_Texture * animal_textures[BASE_ANIMALS];
    SDL_Texture * plant_textures[BASE_PLANTS];

    typedef uint32_t Uint32;

    SDL_Texture * SDL_CreateTexture(SDL_Renderer * renderer, Uint32 format, int access, int w, int h)
    {
        SDL_Texture * t = new SDL_Texture;
        t->w = w;
        t->h = h;
        t->pixels = nullptr;
        return t;
    }

    typedef enum
    {
        SDL_BLENDMODE_NONE = 0x00000000
    } SDL_BlendMode;

    typedef int16_t Sint16;
    typedef uint16_t Uint16;

    typedef struct SDL_Rect
    {
        Sint16 x;
        Sint16 y;
        Uint16 w;
        Uint16 h;
    } SDL_Rect;

    int SDL_SetTextureBlendMode(SDL_Texture * texture, SDL_BlendMode blendMode)
    {
        return 0;
    }
    int SDL_LockTexture(SDL_Texture * texture, const SDL_Rect * rect, void ** pixels, int * pitch)
    {
        *pixels = new char[texture->w * texture->h * 4];
        *pitch = texture->w * 4;
        texture->pixels = *pixels;
        return 0;
    }
    void SDL_UnlockTexture(SDL_Texture * texture)
    {
        delete texture->pixels;
    }
    int SDL_QueryTexture(SDL_Texture * texture, Uint32 * format, int * access, int * w, int * h)
    {
        *w = texture->w;
        *h = texture->h;
        return 0;
    }
    const char * SDL_GetError(void)
    {
        return nullptr;
    }

    typedef enum
    {
        SDL_FLIP_NONE = 0x00000000,       /**< Do not flip */
        SDL_FLIP_HORIZONTAL = 0x00000001, /**< flip horizontally */
        SDL_FLIP_VERTICAL = 0x00000002    /**< flip vertically */
    } SDL_RendererFlip;

    typedef struct SDL_Point
    {
        int x;
        int y;
    } SDL_Point;

    int SDL_RenderCopy(SDL_Renderer * renderer, SDL_Texture * texture, const SDL_Rect * srcrect, const SDL_Rect * dstrect)
    {
        return 0;
    }
    int SDL_RenderCopyEx(SDL_Renderer * renderer, SDL_Texture * texture, const SDL_Rect * srcrect, const SDL_Rect * dstrect, const double angle, const SDL_Point * center, const SDL_RendererFlip flip)
    {
        return 0;
    }
}

SDL_Texture * add_texture_color(SDL_Surface * s, Color c)
{
    return nullptr;
}

enum object_textures_enum
{
    TEXTURE_stone_wall,
    TEXTURE_log_wall,
    TEXTURE_log1_wall,
    TEXTURE_log2_wall,
    TEXTURE_wall_max
};

SDL_Texture * object_textures[TEXTURE_wall_max];
