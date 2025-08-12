#ifndef MENU_H
#define MENU_H

#include "../core/alchemist/npc_talk.h"
#include "../core/alchemist/el_list.h"
#include "text.h"

enum menu_actions
{
    MENU_CANCEL,
    MENU_SAVE_EXIT,
    MENU_EXIT,
    MENU_SAVE,
    MENU_LOAD,
    MENU_HELP,
    MENU_DEBUG,
    MENU_REGAIN,
    MENU_LOUDER,
    MENU_QUIETER,
    MENU_MUSIC,

    MENU_INVENTORY,
    MENU_INV_ELEMENTS,
    MENU_INV_INGREDIENTS,
    MENU_INV_PRODUCT,
    MENU_INV_PLANT,
    MENU_INV_ANIMAL,
    MENU_INV_SCROLL,

    MENU_INV_SOLID,
    MENU_INV_LIGQUID,
    MENU_INV_GAS,

    MENU_CRAFT,
    MENU_ACTION,
    MENU_DRINK,
    MENU_EAT,
    MENU_READ,

    MENU_KNOWLEDGE,
    MENU_KNOWLEDGE_ELEMENTS,
    MENU_KNOWLEDGE_PLANTS,
    MENU_KNOWLEDGE_ANIMALS,

    MENU_ITEMS_GROUP,
    MENU_CLASSES,

    MENU_NPC,
    MENU_NPC_SAY,
    MENU_NPC_ASK,

    MENU_ITEM = 0x1000,
    MENU_NPC_CONV = 0x2000,

};

class Menu_entry : public ListElement
{
  public:
    char * entry;
    bool dynamic_entry;
    enum menu_actions action;
    int value;
    InventoryElement * el;
    Sentence * sentence;
    Menu_entry(const char * e, enum menu_actions a, InventoryElement * _el, int v, Sentence * s);
    ~Menu_entry();
};

class Menu
{
    int index;

  public:
    const char * name;
    Menu_entry * menu_pos;
    ElementsList * entries;

    Menu(const char * n);
    ~Menu();

    void add(const char * e, enum menu_actions a);
    void add(const char * e, enum menu_actions a, int v);
    void add(const char * e, enum menu_actions a, InventoryElement * p_el);
    void add(const char * e, enum Npc_say a, InventoryElement * p_el, Sentence * s);

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
extern Menu * current_menu;
extern Menu * menu_npc;
extern Menu * menu_dialog;
extern Menu * menu_action;

int interact(enum menu_actions a);
void show_menu();
void create_menus();
int menu_interact(int key);

#endif
