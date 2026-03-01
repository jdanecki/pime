#include "playerUI.h"
#include "../client-common/2d/texture.h"
#include "../client-common/ui/menu/menu.h"

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
    conversation_started = false;
}

void PlayerUI::update_item_location(ItemLocation & old_loc, ItemLocation & new_loc)
{
    flip = old_loc.chunk.x > new_loc.chunk.x;
}

bool PlayerUI::check_conversation()
{
	if (in_conversation)
	{
		if ((!conversation_started) && (current_menu != menu_npc))
		{
			show_menu_npc();
			conversation_started = true;
			CONSOLE_LOG("check: conversation started\n");
		}
	}
	else {
		conversation_started = false;
	}
	return Player::check_conversation();
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
