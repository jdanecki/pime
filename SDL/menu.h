#ifndef MENU_H
#define MENU_H

#include "../core/alchemist/npc_talk.h"
#include "text.h"

enum menu_actions
{
    MENU_CANCEL,
    MENU_SAVE_EXIT,
    MENU_EXIT,
    MENU_SAVE,
    MENU_LOAD,
    MENU_HELP,
    MENU_REGAIN,
    MENU_LOUDER,
    MENU_QUIETER,
    MENU_MUSIC,
    MENU_INV_SOLID,
    MENU_INV_LIGQUID,
    MENU_INV_GAS,

    MENU_CRAFT_AXE_BLADE,
    MENU_CRAFT_AXE_HANDLE,
    MENU_CRAFT_AXE,

    MENU_CRAFT_PICKAXE_BLADE,
    MENU_CRAFT_PICKAXE_HANDLE,
    MENU_CRAFT_PICKAXE,

    MENU_CRAFT_KNIFE_BLADE,
    MENU_CRAFT_KNIFE_HANDLE,
    MENU_CRAFT_KNIFE,

    MENU_CRAFT_WALL,
    MENU_CRAFT_HUT,

    MENU_CATEGORIES,

    MENU_NPC_SAY,
    MENU_NPC_ASK,

    MENU_ITEM = 0x1000,
    MENU_NPC_CONV  = 0x2000,

};

class Menu_entry
{
  public:
    SDL_Texture * texture;
    char * entry;
    bool dynamic_entry;
    enum menu_actions action;
    int value;
    InventoryElement * el;
    Sentence * sentence;
    Menu_entry(const char * e, enum menu_actions a, int v, InventoryElement * _el, SDL_Texture * t);
    Menu_entry(const char * e, enum menu_actions a, Sentence * s, InventoryElement * _el);
    ~Menu_entry();
};

class Menu
{
  public:
    const char * name;
    int options;
    int menu_pos;
    int added;
    Menu_entry ** entries;

    bool show_texture;
    bool show_texture_literal;
    Menu(const char * n, int opt);
    ~Menu();

    void add(const char * e, enum menu_actions a);
    void add(const char * e, enum menu_actions a, int val);
    void add(const char * e, enum menu_actions a, int val, InventoryElement * p_el);
    void add(const char * e, enum menu_actions a, InventoryElement * p_el);
    void add(const char * e, enum menu_actions a, SDL_Texture * t, int index, int item_id);
    void add(const char * e, enum Npc_say a, Sentence * s, InventoryElement * p_el);
    int get_val(int v);
    int get_val();
    Sentence * get_sentence();
    InventoryElement * get_el();
    void show();
    void go_down();
    void go_up();
    int interact();
    int handle_item(int i);
};

extern Menu * menu_main;
extern Menu * menu_energy;
extern Menu * menu_help;
extern Menu * menu_help2;
extern Menu * menu_crafting;
extern Menu * current_menu;
extern Menu * menu_npc;
extern Menu * menu_dialog;

int interact(enum menu_actions a);
void show_menu();
void create_menus();
int menu_interact(int key);

#endif
