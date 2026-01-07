#include "npc.h"
#include "menu.h"
#include "../core/networking.h"
#include "texture.h"
#include "window.h"
#include <SDL2/SDL_render.h>
#include <SDL2/SDL_surface.h>
#include <stdio.h>
#include "playerUI.h"

NpcSDL * current_npc;
extern int active_hotbar;


SDL_Texture * create_npc_texture()
{
    BaseElement * b = get_base_element(get_tile_at(player->location));
    ColorRGB c={0,};
    Uint8 mask_r;
    Uint8 mask_g;
    Uint8 mask_b;
    if (b)
    {
        c = b->color;
        mask_r = c.r;
        mask_g = c.g;
        mask_b = c.b;
    }
    else
    {
        mask_r = c.r;
        mask_g = c.g;
        mask_b = c.b;
    }
    SDL_Surface * surface = SDL_CreateRGBSurface(0, 32, 32, 32, 0x00FF0000, 0x0000FF00, 0x000000FF, 0xFF000000);
    SDL_BlitSurface(Player_textures.npc, NULL, surface, NULL);
    SDL_LockSurface(surface);

    SDL_PixelFormat * fmt = surface->format;
    Uint32 * pixels = (Uint32 *)surface->pixels;

    for (int y = 0; y < surface->h; ++y)
    {
        for (int x = 0; x < surface->w; ++x)
        {
            Uint32 * p = pixels + y * surface->w + x;
            Uint8 r, g, b, a;
            SDL_GetRGBA(*p, fmt, &r, &g, &b, &a);

            if (r == g && g == b)
            {
                Uint8 gray = (r + g + b) / 3;
                Uint8 nr = (mask_r * gray) / 255;
                Uint8 ng = (mask_g * gray) / 255;
                Uint8 nb = (mask_b * gray) / 255;
                *p = SDL_MapRGBA(fmt, nr, ng, nb, a);
            }
        }
    }

    SDL_UnlockSurface(surface);
    SDL_Texture * retval = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_FreeSurface(surface);
    return retval;
}

NpcSDL::NpcSDL(Npc data) : Npc(data)
{
    w = 32;
    h = 32;
    texture = create_npc_texture();
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
