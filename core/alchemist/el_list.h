#ifndef EL_LIST_H
#define EL_LIST_H

#include "elements.h"
#ifdef USE_ENET
#include "serialization-enet.h"
#else
#include "serialization-rust.h"
#endif

class ListElement
{
    bool enabled;
  protected:
    Class_id c_id;
  public:
    SerializablePointer<InventoryElement> el;
    ListElement *next, *prev;

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
        return el.get()->tick();
    }
    ListElement(InventoryElement * entry);
    ListElement() : c_id(Class_ListElement), el(nullptr), next(nullptr), prev(nullptr)
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
    virtual size_t get_size() { return sizeof(size_t); } //only uid size
    Class_id get_cid() {  return c_id; }
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
        c_id=Class_KnownElement;
    }

    bool is_known()
    {
        return known;
    }
    void set_known()
    {
        known = true;
        printf("learning %s %d\n", class_name[elid.c_id], elid.id);
    }

    bool check(void * what)
    {
        ElId * i = (ElId *)what;
        return (elid.c_id == i->c_id && elid.id == i->id);
    }
    bool check_class(Class_id id)
    {
        return elid.c_id == id;
    }
    int get_id()
    {
        return elid.id;
    }
    size_t get_size() { return sizeof(KnownElement); }
};

class BaseListElement : public ListElement
{
  public:
    Base * base;
    BaseListElement(Base * base) : base(base) {
        c_id=Class_BaseListElement;
    }
    bool check(void * what)
    {
        int *pid = (int *)what;
        return (*pid == base->id);
    }
    size_t get_size() { return base->get_size() ; }
    void show(bool details = true);
};

class ElementsListIterator
{
    ListElement* le;
  public:
    ElementsListIterator(ListElement *le): le(le) {}
    bool operator!=(ElementsListIterator& other)
    {
        return le != other.le;
    }
    ElementsListIterator operator++()
    {
        le = le->next;
        return *this;
    }
    InventoryElement* operator*()
    {
        return le->el.get();
    }
};

class ElementsList
{
   protected:    
    void virtual copy(ListElement *el) {}

  public:
    void remove_all();
    const char * name;
    int nr_elements;
    ListElement * head;
    ListElement * tail;
    ElementsList(const char * n);
    ElementsList();
    virtual ~ElementsList()
    {
        remove_all();
    }
    ListElement * find(void * what);
    bool virtual find_check(ListElement * el, void * what)
    {
        return el->check(what);
    }
    void show(bool details = true);
    void enable_all();
    void disable_all();
    ListElement * add(ListElement * el);
    ListElement * add_front(ListElement * el);
    void remove(ListElement * el);
    void tick();
    void copy_elements(ElementsList * dst);
    ListElement * get_random();
    
    ElementsListIterator begin() {return head;}
    ElementsListIterator end() {return nullptr;}
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
    InventoryElement * add_front(InventoryElement * el);
    void remove(InventoryElement * el);
};

#endif // EL_LIST_H
