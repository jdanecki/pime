#include "npc.h"
#include "menu.h"
#include "networking.h"
#include "texture.h"
#include "window.h"
#include <SDL2/SDL_render.h>
#include <SDL2/SDL_surface.h>
#include <cstdio>

extern Player * player;
NpcSDL * current_npc;
extern int active_hotbar;

int get_tile_at(int chunk_x, int chunk_y, int x, int y)
{
    if (!world_table[chunk_y][chunk_x])
    {
        printf("missing tile\n");
        return -1;
    }

    return world_table[chunk_y][chunk_x]->table[y][x].tile;
}

NpcSDL::NpcSDL(Npc data) : Npc(data)
{
}

SDL_Texture * NpcSDL::get_texture()
{
    if (!texture)
        init();
    return texture;
}

void NpcSDL::init()
{
    if (!texture)
    {
        int tile = get_tile_at(player->map_x, player->map_y, 0, 0);
        if (tile < 0)
            return;
        BaseElement * b=get_base_element(tile);
        if (b) {
            Color c = b->color; // FIXME
            texture = add_texture_color(Player_textures.npc, c);
        }  else {
            texture = add_texture_color(Player_textures.npc, {125, 125, 125});
        }
    }
}

int npc(menu_actions a)
{
    if (menu_dialog)
    {
        delete menu_dialog;
    }
    SentencesList * list;
    if (a == MENU_NPC_SAY)
        list = sentences;
    else
        list = questions;

    int menu_entries = list->nr_elements;
    list->enable_all();

    InventoryElement * item_at = nullptr;
    if (a == MENU_NPC_ASK)
    {
        if (player->hotbar[active_hotbar])
        {
            list->enable(NPC_Ask_do_you_know_inv_item);
        }
        else
        {
            list->disable(NPC_Ask_do_you_know_inv_item);
            menu_entries--;
        }
        item_at = get_item_at_ppos(player);
        if (item_at)
        {
            list->enable(NPC_Ask_do_you_know_item);
        }
        else
        {
            list->disable(NPC_Ask_do_you_know_item);
            menu_entries--;
        }
    }
    else
    {
        Npc_say sid = NPC_Say_Nothing;
        Sentence * a = dynamic_cast<Sentence *>(sentences->find(&sid));
        a->disable();
        menu_entries--;
    }

    menu_dialog = new Menu("NPC conversation");

    Sentence * sentence = (Sentence *)list->head;
    while (sentence)
    {
        if (sentence->is_enabled())
        {
            switch (sentence->id)
            {
                case NPC_Ask_do_you_know_inv_item:
                    menu_dialog->add(sentence->text, sentence->id, player->hotbar[active_hotbar], sentence);
                    break;
                case NPC_Ask_do_you_know_item:
                    menu_dialog->add(sentence->text, sentence->id, item_at, sentence);
                    break;
                default:
                    menu_dialog->add(sentence->text, sentence->id, nullptr, sentence);
                    break;
            }
        }
        sentence = (Sentence *)sentence->next;
    }
    menu_dialog->add("Cancel", MENU_CANCEL);
    current_menu = menu_dialog;
    return 0;
}
