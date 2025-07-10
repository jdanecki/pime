#ifndef EL_LIST_H
#define EL_LIST_H

#include "elements.h"

class ListElement
{
    bool enabled;

  public:
    InventoryElement * el;
    ListElement * next;

    void add(ListElement * entry);
    void disable()
    {
        enabled = false;
    };
    void enable()
    {
        enabled = true;
    };
    bool is_enabled()
    {
        return enabled;
    };
    virtual void show(bool details = true);
    virtual bool tick()
    {
        return el->tick();
    };
    ListElement(InventoryElement * entry);
    ListElement() : el(nullptr), next(nullptr)
    {
        enable();
    }
    virtual bool check(void * what)
    {
        return what == this;
    }
    virtual ~ListElement() {}
};

class ElementsTable : public ListElement
{
    int count;
    bool * elements;
    Class_id c_id;

  public:
    ElementsTable(int c, Class_id t)
    {
        elements = new bool[c];
        for (int i = 0; i < c; i++)
            elements[i] = false;
        c_id = t;
        count = c;
    }

    bool is_known(int i)
    {
        return elements[i];
    }
    void set_known(int i)
    {
        elements[i] = true;
    }
    void set_all_known()
    {
        for (int i = 0; i < count; i++)
            elements[i] = true;
    }
    bool check(void * what)
    {
        Class_id i = *(Class_id *)what;
        return i == c_id;
    }
};

class BaseTable : public ListElement
{
    int count;
    Base ** base;
    Class_id c_id;

  public:
    BaseTable(int c, Class_id t)
    {
        base = new Base *[c];
        for (int i = 0; i < c; i++)
            base[i] = nullptr;
        c_id = t;
        count = c;
    }
    void add(int i, Base * b)
    {
        base[i] = b;
    }
    void show(bool details)
    {
        for (int i = 0; i < count; i++)
            base[i]->show(details);
    }
    bool check(void * what)
    {
        Class_id i = *(Class_id *)what;
        return i == c_id;
    }
    Base * get_random()
    {
        return base[rand() % count];
    }
    Base * get(int i)
    {
        return base[i];
    }
};

class ElementsList
{
  public:
    const char * name;
    int nr_elements;
    ListElement * head;
    ListElement * tail;
    ElementsList(const char * n);
    ElementsList();
    ListElement * find(void * what);
    bool virtual find_check(ListElement * el, void * what)
    {
        return el->check(what);
    }
    void show(bool details = true);
    void enable_all();
    ListElement * add(ListElement * el);
    void remove(ListElement * el);
    void tick();
};

class InvList : public ElementsList
{
  public:
    InvList(const char * n) : ElementsList(n)
    {
    }
    InvList() : ElementsList("Inventory list")
    {
    }
    //    bool virtual find_at_check(ListElement *el, void * pos) { return false; }
    InventoryElement ** find_form(enum Form f, int * count);

    // FIXME
    // InventoryElement ** find_id(enum Item_id id, int * count);

    InventoryElement * add(InventoryElement * el);
    void remove(InventoryElement * el);
};

#endif // EL_LIST_H
