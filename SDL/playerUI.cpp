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

void PlayerUI::render(int x, int y)
{
    flip = !going_right;
    Renderable::render(x, y);
}

void PlayerUI::update_item_location(ItemLocation &old_loc, ItemLocation &new_loc)
{
    going_right = (old_loc.chunk.x < new_loc.chunk.x) ? true : false;
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
