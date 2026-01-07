#include <dirent.h>
#include <assert.h>

#include "texture.h"

struct player_textures Player_textures;

int tiles_textures_count;
int base_plants_count;
int base_animals_count;
Backend_Texture * tiles_textures;

Backend_Texture ing_textures[ING_COUNT];
Backend_Texture prod_textures[PROD_COUNT];
Backend_Surface scroll_surface;
Backend_Texture animal_textures[BASE_ANIMALS];
Backend_Texture plant_textures[BASE_PLANTS];

Backend_Texture places_textures[PLACES_COUNT];

Backend_Texture load_texture(const char * texture_name)
{
    Backend_Texture texture;
    Backend_Surface loadedSurface = Backend_Image_Load(texture_name);
    if (!Backend_Check_Surface(loadedSurface))
    {
        CONSOLE_LOG("Unable to load texture: %s error: %s\n", texture_name, Backend_Get_Error());
        exit(0);
    }
    else
    {
        texture = Backend_Create_Texture_From_Surface(loadedSurface);

        if (!Backend_Check_Texture(texture))
        {
            CONSOLE_LOG("Unable to create texture: %s error: %s\n", texture_name, Backend_Get_Error());
            exit(0);
        }
        Backend_Free_Surface(loadedSurface);
    }

    return texture;
}

void add_items_texture(int id, const char * file)
{
}

void add_ing_texture(int id, const char * file)
{
    CONSOLE_LOG("adding ing texture: %s\n", file);
    ing_textures[id] = load_texture(file);
}

void add_prod_texture(int id, const char * file)
{
    CONSOLE_LOG("adding product texture: %s\n", file);
    prod_textures[id] = load_texture(file);
}

int add_textures_from_dir(Backend_Texture * to, int i, const char * dir_path)
{
    DIR * dir = opendir(dir_path);

    if (dir == NULL)
    {
        perror("Can't open textures directory'");
        exit(0);
    }
    closedir(dir);

    struct dirent ** namelist;
    int n = scandir(dir_path, &namelist, NULL, alphasort);
    if (n < 0)
        perror("scandir");
    else
    {
        char path[300];
        while (n--)
        {
            if (namelist[n]->d_name[0] == '.' && (namelist[n]->d_name[1] == '\0' || (namelist[n]->d_name[1] == '.' && namelist[n]->d_name[2] == '\0')))
            {
                continue;
            }
            sprintf(path, "%s/%s", dir_path, namelist[n]->d_name);
            CONSOLE_LOG("adding texture: %s\n", path);
            to[i++] = load_texture(path);
            free(namelist[n]);
        }
        free(namelist);
    }

    return i;
}

void load_textures()
{
    Player_textures.npc = Backend_Image_Load("textures/player.png");
    // SDL_PIXELFORMAT_ABGR8888 for surface
    // CONSOLE_LOG("surface format=%x %s rgba=%x argb=%x\n", surface->format->format, SDL_GetPixelFormatName(surface->format->format), SDL_PIXELFORMAT_RGBA8888, SDL_PIXELFORMAT_ARGB8888);
    /*
    bajt 0: R
    bajt 1: G
    bajt 2: B
    bajt 3: A
Bits:   [31..24]   [23..16]   [15..8]   [7..0]
Field:     A          B          G        R
*/
    Player_textures.player = Backend_Create_Texture_From_Surface(Player_textures.npc);

    Player_textures.run_icon = load_texture("textures/gui/run_icon.png");
    Player_textures.sneak_icon = load_texture("textures/gui/sneak_icon.png");

    int i = 0;

    tiles_textures = new Backend_Texture [TILE_TEXTURES];
    tiles_textures_count = add_textures_from_dir(tiles_textures, i, "textures/game_tiles");
    assert(tiles_textures_count == TILE_TEXTURES);

    add_ing_texture(ING_AXE_BLADE, "textures/items/ingredients/axe_blade.png");
    add_ing_texture(ING_AXE_HANDLE, "textures/items/ingredients/axe_handle.png");
    add_ing_texture(ING_KNIFE_BLADE, "textures/items/ingredients/knife_blade.png");
    add_ing_texture(ING_KNIFE_HANDLE, "textures/items/ingredients/knife_handle.png");
    add_ing_texture(ING_PICKAXE_BLADE, "textures/items/ingredients/pickaxe_blade.png");
    add_ing_texture(ING_PICKAXE_HANDLE, "textures/items/ingredients/pickaxe_handle.png");
    add_ing_texture(ING_HOE_BLADE, "textures/items/ingredients/hoe_blade.png");
    add_ing_texture(ING_HOE_HANDLE, "textures/items/ingredients/hoe_handle.png");
    add_ing_texture(ING_WALL, "textures/items/ingredients/wall.png");
    add_ing_texture(ING_MEAT, "textures/items/ingredients/meat.png");
    add_ing_texture(ING_LOG, "textures/items/ingredients/log.png");
    add_ing_texture(ING_STICK, "textures/items/ingredients/stick.png");
    add_ing_texture(ING_TINDER, "textures/items/ingredients/tinder.png");
    add_ing_texture(ING_FRUIT, "textures/items/ingredients/fruit.png");
    add_ing_texture(ING_SEED, "textures/items/ingredients/seed.png");

    add_prod_texture(PROD_AXE, "textures/items/products/axe.png");
    add_prod_texture(PROD_KNIFE, "textures/items/products/knife.png");
    add_prod_texture(PROD_PICKAXE, "textures/items/products/pickaxe.png");
    add_prod_texture(PROD_HOE, "textures/items/products/hoe.png");
    add_prod_texture(PROD_FIRE, "textures/items/products/fire.png");
    add_prod_texture(PROD_ROASTED_MEAT, "textures/items/products/roasted_meat.png");
    add_prod_texture(PROD_FRUIT_SALAD, "textures/items/products/fruit_salad.png");
    add_prod_texture(PROD_SEEDLING, "textures/items/products/seedling.png");

    add_prod_texture(PROD_HUT, "textures/objects/hut.png");
    add_prod_texture(PROD_TENT, "textures/objects/tent.png");

    i = 0;
    base_plants_count = add_textures_from_dir(plant_textures, i, "textures/plants");
    assert(base_plants_count == BASE_PLANTS);

    i=0;
    base_animals_count = add_textures_from_dir(animal_textures, i, "textures/animals");
    assert(base_animals_count == BASE_ANIMALS);

    scroll_surface = Backend_Image_Load("textures/scroll.png");

    places_textures[PLACE_FIELD] = load_texture("textures/objects/field.png");
}
