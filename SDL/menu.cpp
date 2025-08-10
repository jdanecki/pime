#include "menu.h"
#include "../core/alchemist/elements.h"
#include "../core/player.h"
#include "../core/tiles.h"
#include "../core/world.h"
#include "music.h"
#include "networking.h"
#include "npc.h"
#include "text.h"
#include "texture.h"
#include "window.h"
#include <cstddef>
#include <cstdio>
#include <cstdlib>
#include "dialog/d_craft.h"
#include "player_actions.h"
#include <cassert>

extern class Player * player;
extern int active_hotbar;
extern NetClient * client;

//Menu * menu_music;
Menu * menu_main;
Menu * menu_help;
Menu * current_menu;
Menu * menu_inventory_categories;
Menu * menu_inventory_elements;
Menu * menu_inventory;
Menu * menu_inventory_elements_form;
Menu * menu_inventory_class;

Menu * menu_npc;
Menu * menu_dialog;
Menu * menu_action;

void load(char with_player);
void save(char with_player);

Menu::Menu(const char * n)
{
    name = n;            
    entries = new ElementsList(n);
    menu_pos = nullptr;
    index = 0;
}

Menu::~Menu()
{
    delete entries;
}

void Menu::add(const char * e, enum menu_actions a)
{    
    entries->add(new Menu_entry(e, a, nullptr, 0, nullptr));
    if (!menu_pos) menu_pos = dynamic_cast<Menu_entry*>(entries->head);
}

void Menu::add(const char *e, menu_actions a, int v)
{
    entries->add(new Menu_entry(e, a, nullptr, v, nullptr));
    if (!menu_pos) menu_pos = dynamic_cast<Menu_entry*>(entries->head);
}

void Menu::add(const char * e, enum Npc_say a,  InventoryElement * p_el, Sentence * s)
{
    enum menu_actions conv = (enum menu_actions)((int)MENU_NPC_CONV + (int)a);
    entries->add(new Menu_entry(e, conv, p_el, 0, s));
    if (!menu_pos) menu_pos = dynamic_cast<Menu_entry*>(entries->head);
}

void Menu::add(const char * e, enum menu_actions a, InventoryElement * p_el)
{    
    entries->add(new Menu_entry(e, a, p_el, 0, nullptr));
    if (!menu_pos) menu_pos = dynamic_cast<Menu_entry*>(entries->head);
}

int Menu::get_val(int v)
{
    //FIXME
    /*
    for (int i = 0; i < added; i++)
    {
        if (entries[i]->action == v)
            return entries[i]->value;
    }*/
    return 0;
}

int Menu::get_val()
{
    return menu_pos->value;
}

Sentence * Menu::get_sentence()
{
    return menu_pos->sentence;
}

InventoryElement * Menu::get_el()
{
    return menu_pos->el;
}

void Menu::show()
{
    int window_width;
    int window_height;
    int i;
    int game_size;

    SDL_GetWindowSize(main_window, &window_width, &window_height);

    if (window_width < window_height)
        game_size = window_width;
    else
        game_size = window_height;

    int menu_entries = entries->nr_elements;
    int menu_opt_size = game_size / ( menu_entries+ 3);
    int mody;
    int mody2;
    // printf("menu_entries = %d\n", menu_entries);

    if (menu_entries % 2)
    {
        mody = (game_size / 2) - ((menu_opt_size * (menu_entries / 2) + menu_opt_size / 2));
        mody2 = (game_size / 2) + ((menu_opt_size * (menu_entries / 2) + menu_opt_size / 2));
    }
    else
    {
        mody = (game_size / 2) - (menu_opt_size * (menu_entries / 2));
        mody2 = (game_size / 2) + (menu_opt_size * (menu_entries / 2));
    }

    int modx = int((game_size / 2)) - (0.4 * game_size);
    int modx2 = int((game_size / 2)) + (0.6 * game_size);

    SDL_Rect rect = {modx, mody, modx2 - modx, mody2 - mody};
    SDL_SetRenderDrawColor(renderer, 0, 0, 255, 100);
    SDL_RenderFillRect(renderer, &rect);

    int mody3 = mody + (((index + 1) * menu_opt_size) - (menu_opt_size));
    int mody4 = mody + ((index + 1) * menu_opt_size);

    // THIS IS THE SELECT
    // if (added < 10)
    {
        draw_rectangle(modx, mody3, modx2 - modx, mody4 - mody3, SDL_Color{0, 255, 255, 255}, SDL_Color{0, 255, 255, 255}, SDL_Color{}, SDL_Color{});
        draw_rectangle(modx, mody3, modx2 - modx, mody4 - mody3, SDL_Color{}, SDL_Color{}, SDL_Color{0, 255, 255, 255}, SDL_Color{0, 255, 255, 255});
    }
    /* else
     {
         if (added % 2)
         {
             draw_rectangle(modx, game_size / 2 - menu_opt_size / 2, modx2 - modx, menu_opt_size, SDL_Color{0, 255, 255, 255}, SDL_Color{0, 255, 255, 255}, SDL_Color{}, SDL_Color{});
             draw_rectangle(modx, game_size / 2 - menu_opt_size / 2, modx2 - modx, menu_opt_size, SDL_Color{}, SDL_Color{}, SDL_Color{0, 255, 255, 255}, SDL_Color{0, 255, 255, 255});
         }
         else
         {
             draw_rectangle(modx, game_size / 2, modx2 - modx, menu_opt_size, SDL_Color{0, 255, 255, 255}, SDL_Color{0, 255, 255, 255}, SDL_Color{}, SDL_Color{});
             draw_rectangle(modx, game_size / 2, modx2 - modx, menu_opt_size, SDL_Color{}, SDL_Color{}, SDL_Color{0, 255, 255, 255}, SDL_Color{0, 255, 255, 255});
         }
     }
 */
    //  if (added < 10)
    {
        SDL_Rect rect3 = {modx, mody - menu_opt_size, modx2 - modx, mody4 - mody3};
        SDL_SetRenderDrawColor(renderer, 150, 0, 150, 100);
        SDL_RenderFillRect(renderer, &rect3);
        write_text(modx, mody - menu_opt_size, name, Yellow, game_size / 27, menu_opt_size);
    }
    /*else
    {
        SDL_Rect rect3 = {modx, mody + (-menu_pos + added / 2) * menu_opt_size - menu_opt_size, modx2 - modx, menu_opt_size};
        SDL_SetRenderDrawColor(renderer, 150, 0, 150, 100);
        SDL_RenderFillRect(renderer, &rect3);
        write_text(modx, mody + (-menu_pos + added / 2) * menu_opt_size - menu_opt_size, name, Yellow, game_size / 27, menu_opt_size);
    }*/
    Menu_entry * menu_entry= (Menu_entry*)(entries->head);

    for (i = 0; i < menu_entries; i++)
    {
        SDL_Rect rect;
        int text_y = mody + i * menu_opt_size;
        int text_x = modx;
        //   if (added < 10)
        {
            rect.x = modx;
            rect.y = mody + i * menu_opt_size;
            rect.w = menu_opt_size;
            rect.h = menu_opt_size;
        }
        /* else
         {
             rect.x = modx;
             rect.y = mody + (i - menu_pos + added / 2) * menu_opt_size;
             rect.w = menu_opt_size;
             rect.h = menu_opt_size;
         }*/
        //FIXME
        /*if (entries[i]->texture)
        {
            SDL_RenderCopy(renderer, entries[i]->texture, NULL, &rect);
            text_x += menu_opt_size;
        }
        if (show_texture)
        {
            // TODO fix that
            // SDL_Texture *_texture = el[i]->get_texture();
            // SDL_RenderCopy(renderer, _texture, NULL, &rect);
            // text_x+=menu_opt_size;
        }*/
        //  if (added >= 10)
        //    text_y = mody + (i - menu_pos + added / 2) * menu_opt_size;

        write_text(text_x, text_y, menu_entry->entry, White, game_size / 27, menu_opt_size);
        menu_entry=(Menu_entry*)(menu_entry->next);
    }
}

Menu_entry::Menu_entry(const char * e, menu_actions a, InventoryElement * _el, int v, Sentence * s)
{    
    action = a;
    el = _el;
    sentence = s;    
    value=v;
    if (el && !s)
    {
        entry = new char[64];
        sprintf(entry, "%s %s", e, el->get_name());
        dynamic_entry = true;
    }
    else
    {
        entry = (char *)e;
        dynamic_entry = false;
    }
}

Menu_entry::~Menu_entry()
{
    if (dynamic_entry)
        delete entry;
}

void create_menus()
{
    menu_main = new Menu("Main");
    menu_main->add("Exit", MENU_EXIT);
    // add("Save & Exit", MENU_SAVE_EXIT);
    // add("Save", MENU_SAVE);
    // add("Load", MENU_LOAD);
    menu_main->add("Help", MENU_HELP);
   // menu_main->add("Change music volume", MENU_MUSIC);
    menu_main->add("Cancel", MENU_CANCEL);

    menu_help = new Menu("Help 1");
    menu_help->add("esc - main menu", MENU_CANCEL);
    menu_help->add("F1 - show item info", MENU_CANCEL);
    menu_help->add("F2 - show item info on server", MENU_CANCEL);

    menu_help->add("F3 - show chunk info", MENU_CANCEL);
    menu_help->add("F4 - show chunk info on server", MENU_CANCEL);

    menu_help->add("F5 - trace network", MENU_CANCEL);
    menu_help->add("F6 - trace network on server", MENU_CANCEL);
    menu_help->add("F11 - resize", MENU_CANCEL);

    menu_help->add("1-9,0 - hotbar", MENU_CANCEL);
    menu_help->add("enter - use item", MENU_CANCEL);
    menu_help->add("q - drop item", MENU_CANCEL);
    menu_help->add("` - previous item", MENU_CANCEL);
    menu_help->add("tab - next item", MENU_CANCEL);
    menu_help->add("= - select hotbar", MENU_CANCEL);
    menu_help->add("minus - deselect hotbar", MENU_CANCEL);
    menu_help->add("shift/control - sneak/run", MENU_CANCEL);
    menu_help->add("arrows - move", MENU_CANCEL);
    menu_help->add("n - NPC", MENU_CANCEL);
    menu_help->add("c - Craft", MENU_CRAFT);
    menu_help->add("a - Action", MENU_ACTION);

    //   menu_help->add("l - devmenu", MENU_CANCEL);
    menu_help->add("i - inventory", MENU_CANCEL);
    //  menu_help->add("v - clear statusline", MENU_CANCEL);
    //   menu_help->add("g - terrain break", MENU_CANCEL);
    //  menu_help->add("r - remove from hotbar", MENU_CANCEL);

    //  menu_help->add("F5 - autoexplore", MENU_CANCEL);
    //  menu_help->add("F4 - item info at player", MENU_CANCEL);

  /*  menu_music = new Menu("Music", 3);
    menu_music->add("+5 Volume", MENU_LOUDER);
    menu_music->add("-5 Volume", MENU_QUIETER);
    menu_music->add("Cancel", MENU_CANCEL);
*/
    menu_inventory_categories = new Menu("Inventory categories");
    menu_inventory_categories->add("Elements", MENU_INV_ELEMENTS, Class_Element);
    menu_inventory_categories->add("Ingredients", MENU_INV_INGREDIENTS, Class_Ingredient);
    menu_inventory_categories->add("Products", MENU_INV_PRODUCT, Class_Product);
    menu_inventory_categories->add("Plants", MENU_INV_PLANT, Class_Plant);
    menu_inventory_categories->add("Animals", MENU_INV_ANIMAL, Class_Animal);
    menu_inventory_categories->add("Scrolls", MENU_INV_SCROLL, Class_Scroll);
    menu_inventory_categories->add("Cancel", MENU_CANCEL);

    menu_inventory_elements = new Menu("Inventory elements");
    menu_inventory_elements->add("Solid form", MENU_INV_SOLID, Form_solid);
    menu_inventory_elements->add("Liquid form", MENU_INV_LIGQUID, Form_liquid);
    menu_inventory_elements->add("Gas form", MENU_INV_GAS, Form_gas);
    menu_inventory_elements->add("Cancel", MENU_CANCEL);

    menu_npc = new Menu("NPC");
    menu_npc->add("Talk to NPC", MENU_NPC_SAY);
    menu_npc->add("Ask NPC", MENU_NPC_ASK);
    menu_npc->add("Cancel", MENU_CANCEL);

    menu_action = new Menu("Action");
    menu_action->add("Drink", MENU_DRINK);
    menu_action->add("Eat", MENU_EAT);
    menu_action->add("Read", MENU_READ);
    menu_action->add("Cancel", MENU_CANCEL);
}

//create menu with selected form
Menu * create_inv_form(enum Form f)
{
    int count = 0;
    InventoryElement ** elements_with_form = player->inventory->find_form(f, &count);
    if (!count)
        return nullptr;

    if (menu_inventory_elements_form)
    {
        delete menu_inventory_elements_form;
    }
    char * menu_name = new char[50];
    sprintf(menu_name, "Inventory: %s", Form_name[f]);

    menu_inventory_elements_form = new Menu(menu_name);
    // int menu_index = 0;
    for (int i = 0; i < count; i++)
    {
        // FIXME add texture
        // menu_inventory_categories2->add(base_elements[i]->name, MENU_CATEGORIES, items_textures[i], menu_index, i);
        //        menu_index++;
        //fixme group elements with the same base id
        menu_inventory_elements_form->add("*", MENU_ITEMS_GROUP, elements_with_form[i]);

    }
    return menu_inventory_elements_form;
}

Menu * create_inv_category_classes(enum Class_id c)
{
    int count = 0;
    InventoryElement ** elements_with_class = player->inventory->find_class(c, &count);
    if (!count)
        return nullptr;

    if (menu_inventory_class)
    {
        delete menu_inventory_class;
    }
    char * menu_name = new char[50];
    sprintf(menu_name, "Inventory: %s", class_name[c]);

    menu_inventory_class = new Menu(menu_name);
    for (int i = 0; i < count; i++)
    {
        menu_inventory_class->add("*", MENU_CLASSES, elements_with_class[i]);
    }
    return menu_inventory_class;
}

// create menu for elements with the same base id
void create_inv_menu(int id)
{
    printf("szukam %d\n", id);
    int c = 0;
    InventoryElement ** i_el = player->inventory->find_id(id, &c);
    if (i_el)
    {
        printf("found %d elements\n", c);

        if (menu_inventory)
            delete menu_inventory;

        menu_inventory = new Menu("Which element?");
        Element ** el = (Element **)i_el;
        for (int i = 0; i < c; i++)
        {
            printf("%s\n", el[i]->get_name());
            menu_inventory->add("->", (menu_actions)(MENU_ITEM + i), el[i]);
        }
        free(el);
        current_menu = menu_inventory;
        return;
    }
    current_menu = nullptr;
}

void Menu::go_down()
{
    menu_pos=(Menu_entry*)(menu_pos->next);
    index++;
    if (!menu_pos) {
        menu_pos = (Menu_entry*)(entries->head);
        index = 0;
    }
}

void Menu::go_up()
{
    menu_pos=(Menu_entry*)(menu_pos->prev);
    index--;
    if (!menu_pos) {
        menu_pos = (Menu_entry*)(entries->head);
        index = 0;
    }
}

int menu_interact(int key)
{
    switch (key)
    {
        case SDLK_ESCAPE:
        {
            if (current_menu)
                current_menu = NULL;
            else
                current_menu = menu_main;
            return 1;
        }

        case SDLK_RETURN:
        {
            if (current_menu)
            {
                if (current_menu->interact())
                {
                    current_menu = NULL;
                    return 1;
                }
            }
            break;
        }
        case SDLK_i:
        {
            player->inventory->show();
            if (!current_menu)
                current_menu = menu_inventory_categories;
            else if (current_menu == menu_inventory_categories)
                current_menu = NULL;
            return 1;
        }

        case SDLK_n:
        {
            if (!current_menu)
                current_menu = menu_npc;
            else if (current_menu == menu_npc)
                current_menu = NULL;

            return 1;
        }

        case SDLK_DOWN:
        {
            if (current_menu)
                current_menu->go_down();
            break;
        }
        case SDLK_UP:
        {
            if (current_menu)
                current_menu->go_up();
            break;
        }
        case SDLK_a:
        {
            if (!current_menu)
                current_menu = menu_action;
            else if (current_menu == menu_action)
                current_menu = NULL;

            return 1;
        }
    }
    return current_menu ? 1 : 0;
}

//add item from menu to hotbar
int Menu::handle_item(int i)
{
    if (active_hotbar >= 0)
    {
        InventoryElement * el = menu_pos->el;

        for (int h = 0; h < 10; h++)
        {
            if (player->hotbar[h] == el)
                return 0;
        };
        player->hotbar[active_hotbar] = el;
    }
    return 1;
}
/*
void action(menu_actions a)
{
    InventoryElement * el=player->hotbar[active_hotbar];
    if (!el) return;
    Product * product=dynamic_cast<Product *>(el);
    if (!product) return;
    printf("action: %d -> %s\n", product->actions, Product_action_names[product->actions]);

}
*/

int Menu::interact()
{
    menu_actions a = menu_pos->action;

    if (a & MENU_ITEM)
        return menu_inventory->handle_item(a & ~MENU_ITEM);
    if (a & MENU_NPC_CONV)
    {
        return player->conversation(current_npc, menu_dialog->get_sentence(), menu_dialog->get_el());
    }

    switch (a)
    {            
        case MENU_ITEMS_GROUP:
        {// get base id
            //create_inv_menu((Item_id)(menu_inventory_elements_form->get_val()));
            return 0;
        }
            /*
                    case MENU_MUSIC:
                        current_menu=menu_music;
                        return 0;
                    case MENU_REGAIN:
                        player.hunger+=100;
                        player.thirst+=100;
                        return 0;*/
        case MENU_SAVE:
            save(1);
            break;

        case MENU_SAVE_EXIT:
            save(1);
        case MENU_EXIT:
            SDL_Quit();
            Mix_Quit();
            exit(0);
        case MENU_LOAD:
            load(1);
            break;

        case MENU_HELP:
            current_menu = menu_help;
            return 0;
        case MENU_CRAFT:
            d_craft.show ^= 1;
            return 1; // hide menu

        case MENU_INV_ELEMENTS:
            current_menu = menu_inventory_elements;
            return 0;

        case MENU_INV_SOLID:
        case MENU_INV_LIGQUID:
        case MENU_INV_GAS:
            current_menu = create_inv_form((Form)menu_inventory_elements->get_val());
            return 0;

        case MENU_INV_INGREDIENTS:
        case MENU_INV_PRODUCT:
        case MENU_INV_PLANT:
        case MENU_INV_ANIMAL:
        case MENU_INV_SCROLL:
            current_menu = create_inv_category_classes((Class_id)menu_inventory_categories->get_val());
            return 0;

        case MENU_LOUDER:
            Mix_Volume(0, Mix_Volume(0, -1) + 5);
            Mix_Volume(1, Mix_Volume(1, -1) + 5);
            printf("%d\n%d\n", Mix_Volume(1, -1), Mix_Volume(0, -1));
            return 0;

        case MENU_QUIETER:
            Mix_Volume(0, Mix_Volume(0, -1) - 5);
            Mix_Volume(1, Mix_Volume(1, -1) - 5);
            printf("%d\n%d\n", Mix_Volume(1, -1), Mix_Volume(0, -1));
            return 0;

        case MENU_NPC_SAY:
        case MENU_NPC_ASK:
            return npc(a);

        case MENU_ACTION:
            current_menu = menu_action;
            return 0;

        case MENU_DRINK:
            action_tile(PLAYER_DRINK, player->map_x, player->map_y, player->x, player->y);
            return 0;
        case MENU_EAT:
            action_tile(PLAYER_EAT, player->map_x, player->map_y, player->x, player->y);
            return 0;
        case MENU_READ:
            action_tile(PLAYER_READ, player->map_x, player->map_y, player->x, player->y);
            return 0;

        default:
            return 1; // hide menu
    }
    return 1; // hide menu
}
