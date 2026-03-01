#include "menu_debug.h"

Menu * create_menu_debug()
{
    Menu * m = new Menu("Help - debug");
    m->add("F1 - show item info", MENU_CANCEL);
    m->add("F2 - show item info on server", MENU_CANCEL);

    m->add("F3 - show chunk info", MENU_CANCEL);
    m->add("F4 - show chunk info on server", MENU_CANCEL);

    m->add("F5 - trace network", MENU_CANCEL);

    m->add("F6 - show player", MENU_CANCEL);
    m->add("F7 - autoexplore", MENU_CANCEL);
    return m;
}
