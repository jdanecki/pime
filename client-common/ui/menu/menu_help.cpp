#include "menu_help.h"

Menu * create_menu_help()
{
    Menu * m = new Menu("Help");
    m->add("Debug options", MENU_DEBUG);
    m->add("ESC - main menu", MENU_CANCEL);

    m->add("1-9,0 - hotbar", MENU_CANCEL);
    m->add("enter - use item", MENU_CANCEL);
    m->add("q - drop item", MENU_CANCEL);
    m->add("e - pickup item", MENU_CANCEL);
    m->add("` - previous item", MENU_CANCEL);
    m->add("tab - next item", MENU_CANCEL);
    m->add("= - select hotbar", MENU_CANCEL);
    m->add("minus - deselect hotbar", MENU_CANCEL);
    m->add("shift/control - sneak/run", MENU_CANCEL);
    m->add("wasd/arrows - move", MENU_CANCEL);
    m->add("n - NPC", MENU_CANCEL);
    m->add("c - Craft", MENU_CANCEL);
    m->add("z - Action", MENU_CANCEL);
    m->add("i - inventory", MENU_CANCEL);
    m->add("k - knowledge", MENU_CANCEL);

    /*m->add("l - devmenu", MENU_CANCEL);
    m->add("v - clear statusline", MENU_CANCEL);
    m->add("g - terrain break", MENU_CANCEL);
    m->add("r - remove from hotbar", MENU_CANCEL);
*/
    m->add("F4 - item info at player", MENU_CANCEL);

    return m;
}