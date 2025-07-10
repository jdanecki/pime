#ifndef SHOW_LIST_H
#define SHOW_LIST_H

#include "../alchemist/el_list.h"

class Show_el : public ListElement
{
  public:
    char c;
    bool selected;
    ListElement * l_el;
    Show_el(char _c, ListElement * _el);
    void show(bool details = true);
};

class Show_list : public ElementsList
{
    char prompt;

  public:
    Show_list(char p) : ElementsList("select list")
    {
        prompt = p;
    }
    ListElement * select_el();
    bool multi_select();
    void unselect_all();
    bool find_check(ListElement * el, void * what);
};

extern const char * colorGray;
extern const char * colorRed;
extern const char * colorRedBold;
extern const char * colorGreen;
extern const char * colorGreenBold;
extern const char * colorYellow;
extern const char * colorYellowBold;
extern const char * colorBlue;
extern const char * colorMagenta;
extern const char * colorCyan;
extern const char * colorWhite;
extern const char * colorNormal;
extern const char * clrscr;

int kbhit();
char wait_key(char prompt);
ListElement * select_list_element(ElementsList * list);
InventoryElement * select_element(InvList * list);
bool select_inventory2(int count, InventoryElement ** el2);

#endif // SHOW_LIST_H
