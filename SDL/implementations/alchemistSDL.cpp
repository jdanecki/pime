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
}

SDL_Texture * ElementSDL::get_texture()
{
    switch(get_form())
    {
        case Form_solid:
            return solid_items_textures[get_base()->id % SOLID_ELEMENTS];
        case Form_liquid:
            return liquid_items_textures[get_base()->id % LIQUID_ELEMENTS];
        case Form_gas:
            return gas_items_textures[get_base()->id % GAS_ELEMENTS];
    }
    return nullptr;
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
