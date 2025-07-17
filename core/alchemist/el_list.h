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
        elid.id=i;
        known=false;
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
        ElId *i = (ElId *)what;
        return (elid.c_id == i->c_id && elid.id == i->id);
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
    void disable_all();
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
    // InventoryElement ** find_form(enum Form f, int * count);

    // FIXME jacek
    // InventoryElement ** find_id(enum Item_id id, int * count);

    InventoryElement * add(InventoryElement * el);
    void remove(InventoryElement * el);
};

#endif // EL_LIST_H
