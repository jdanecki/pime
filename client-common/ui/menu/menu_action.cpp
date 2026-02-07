#include "menu_action.h"

Menu * create_menu_action()
{
    Menu * m = new Menu("Action");
    m->add("Drink", MENU_DRINK);
    m->add("Eat", MENU_EAT);
    m->add("Read", MENU_READ);
    m->add("Check", MENU_CHECK);
    m->add("Task/Ask Npc", MENU_NPC);
    return m;
}