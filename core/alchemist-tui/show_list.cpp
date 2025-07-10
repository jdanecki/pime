#include "show_list.h"
#include "../npc.h"
#include "game_time.h"
#include <sys/ioctl.h>
#include <unistd.h>

const char * colorGray = "\033[1;30m";
const char * colorRed = "\033[2;31m";
const char * colorRedBold = "\033[1;31m";
const char * colorGreen = "\033[2;32m";
const char * colorGreenBold = "\033[1;32m";
const char * colorYellow = "\033[2;33m";
const char * colorYellowBold = "\033[1;33m";
const char * colorBlue = "\033[2;34m";
const char * colorMagenta = "\033[2;35m";
const char * colorCyan = "\033[2;36m";
const char * colorWhite = "\033[1;37m";
const char * colorNormal = "\033[0m";
const char * clrscr = "\033[H\033[J";

extern Npc * current_npc;
extern Player * player;

int kbhit()
{
    int lf;

    if (ioctl(0, FIONREAD, &lf) == 0)
        return lf;
    else
        return -1;
}

char wait_key(char prompt)
{
    printf("\r%s %s [%c] > ", game_time->get_time(), current_npc ? current_npc->get_name() : " ", prompt);
    while (!kbhit())
    {
        usleep(1000);
    }
    char c;
    if (read(0, &c, 1) == 1)
    {
        printf("%c\n", c);
        return c;
    }
    else
        return 0;
}

Show_el::Show_el(char _c, ListElement * _el)
{
    c = _c;
    l_el = _el;
    selected = false;
}

void Show_el::show(bool details)
{
    printf("%c (%c) - ", c, selected ? '*' : ' ');
    l_el->show(details);
}

bool Show_list::find_check(ListElement * _el, void * what)
{
    Show_el * s_el = (Show_el *)_el;
    char * c = (char *)what;
    if (s_el->c == *c)
        return true;
    return false;
}

ListElement * Show_list::select_el()
{
    char c = wait_key(prompt);
    Show_el * f = (Show_el *)find(&c);
    if (f)
        return f->l_el;
    return nullptr;
}

bool Show_list::multi_select()
{
    char c = 0;
    bool sel = 0;
    printf("z - zakończ selekcje\n");
    while (1)
    {
        c = wait_key(prompt);
        if (c == 'z')
            break;
        Show_el * f = (Show_el *)find(&c);
        if (f)
        {
            f->selected ^= true;
            sel = true;
        }
        show(false);
        printf("z - zakończ selekcje\n");
    }
    return sel;
}

void Show_list::unselect_all()
{
    ListElement * cur = head;
    while (cur)
    {
        Show_el * _el = (Show_el *)cur;
        _el->selected = false;
        cur = cur->next;
    }
}

ListElement * select_list_element(ElementsList * list)
{
    ListElement * elems = list->head;
    Show_list * show_cat = new Show_list('e');
    char k = 'a';

    while (elems)
    {
        if (elems->is_enabled())
        {
            show_cat->add(new Show_el(k, elems));
            k++;
        }
        elems = elems->next;
    }
    printf("%s", colorCyan);
    show_cat->show(false);
    elems = show_cat->select_el();
    if (!elems)
        return nullptr;
    return elems;
}

InventoryElement * select_element(InvList * list)
{
    ListElement * elems = select_list_element(list);
    if (!elems)
        return nullptr;

    if (elems->el)
    {
        printf("%s%s", colorNormal, colorGreenBold);
        printf("selected %s\n", elems->el->get_name());
        return elems->el;
    }

    return nullptr;
}

bool select_inventory2(int count, InventoryElement ** el2)
{
    ListElement * inv = player->inventory->head;
    Show_list * show_cat = new Show_list('i');
    char k = 'a';

    while (inv)
    {
        show_cat->add(new Show_el(k, inv));
        k++;
        inv = inv->next;
    }
    printf("%s", colorCyan);
    show_cat->show(false);
    bool sel = show_cat->multi_select();
    if (!sel)
        return false;

    printf("%swybrane zasoby: ", colorGreen);
    Show_el * el = (Show_el *)show_cat->head;
    int i = 0;

    while (el)
    {
        if (el->selected)
        {
            printf("%s ", el->l_el->el->get_name());
            el2[i]=el->l_el->el;
            i++;
        }
        el = (Show_el *)el->next;
    }
    if (i == 2)
    {
        puts("");
        return true;
    }

    return false;
}
