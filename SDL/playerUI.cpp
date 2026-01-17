#include "playerUI.h"
#include "../client-common/texture.h"

SDL_Texture * PlayerUI::get_texture()
{
    texture_created = true;
    return Player_textures.player;
}

PlayerUI::PlayerUI(Player data) : Player(data)
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
        if (le->get_el())
            player->hotbar[i] = (InventoryElement *)le->get_el();
        le = le->next;
        i++;
    }
}
