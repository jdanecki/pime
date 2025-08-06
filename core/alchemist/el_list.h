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
    }
    void enable()
    {
        enabled = true;
    }
    bool is_enabled()
    {
        return enabled;
    }
    virtual void show(bool details = true);
    virtual bool tick()
    {
        return el->tick();
    }
    ListElement(InventoryElement * entry);
    ListElement() : el(nullptr), next(nullptr)
    {
        enable();
    }
    virtual bool check(void * what)
    {
        return what == this;
    }
    virtual ~ListElement()
    {
    }
};

struct ElId
{
    Class_id c_id;
    int id;
};

class KnownElement : public ListElement
{
    ElId elid;
    bool known;

  public:
    KnownElement(Class_id t, int i)
    {
        elid.c_id = t;
        elid.id = i;
        known = false;
    }

    bool is_known()
    {
        return known;
    }
    void set_known()
    {
        known = true;
    }

    bool check(void * what)
    {
        ElId * i = (ElId *)what;
        return (elid.c_id == i->c_id && elid.id == i->id);
    }
};

class ElementsList
{
    void remove_all();
  public:
    const char * name;
    int nr_elements;
    ListElement * head;
    ListElement * tail;
    ElementsList(const char * n);
    ElementsList();
    virtual ~ElementsList() { remove_all(); }
    ListElement * find(void * what);
    bool virtual find_check(ListElement * el, void * what)
    {
        return el->check(what);
    }
    void show(bool details = true);
    void enable_all();
    void disable_all();
    ListElement * add(ListElement * el);
    void remove(ListElement * el);    
    void tick();
};

typedef bool (*FindFunc)(InventoryElement * el, void * arg);

class InvList : public ElementsList
{
    InventoryElement ** find_by_fun(FindFunc fun, void * arg, int * count);

  public:
    InvList(const char * n) : ElementsList(n)
    {
    }
    InvList() : ElementsList("Inventory list")
    {
    }
  //  bool virtual find_at_check(ListElement *el, void * pos) { return false; }
    InventoryElement ** find_form(enum Form f, int * count);
    InventoryElement ** find_class(enum Class_id cl, int * count);
    InventoryElement ** find_id(int id, int * count);
    InventoryElement * add(InventoryElement * el);
    void remove(InventoryElement * el);
};

#endif // EL_LIST_H
