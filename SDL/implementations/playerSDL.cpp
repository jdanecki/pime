#include "playerSDL.h"
#include "../texture.h"

SDL_Texture * PlayerSDL::get_texture()
{
    return Player_textures.player;
}

PlayerSDL::PlayerSDL(Player data) : Player(data)
{
    for (int i = 0; i < 10; i++)
    {
        hotbar[i] = NULL;
        craftbar[i] = 0;
    }
}

void update_hotbar()
{
    // FIXME - remove/add only one element
    for (int i = 0; i < 10; i++)
        player->hotbar[i] = nullptr;
    ListElement * le = player->inventory.head;
    int i = 0;
    while (le)
    {
        if (i >= 10)
            break;
        if (le->el.get())
            player->hotbar[i] = (InventoryElement*)le->el.get();
        le = le->next;
        i++;
    }
}

