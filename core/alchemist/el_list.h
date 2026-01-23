#ifndef EL_LIST_H
#define EL_LIST_H

#include "elements/base.h"
#include "elements/inventory_element.h"

class ListElement
{
    bool enabled;
    SerializablePointer<NetworkObject> el;

  protected:
    Class_id c_id;

  public:
    ListElement *next, *prev;

    void add(ListElement * entry);
    void disable();
    void enable();
    bool is_enabled();
    virtual void show(bool details = true);
    virtual bool tick();
    ListElement(NetworkObject * entry);
    ListElement();
    virtual bool check(void * what);
    virtual ~ListElement();
    virtual size_t get_size(); // only uid size
    Class_id get_cid();
    NetworkObject * get_el();
};

class BaseListElement : public ListElement
{
  public:
    BaseListElement(Base * base);
    bool check(void * what);
    size_t get_size();
    void show(bool details = true);
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
    KnownElement(Class_id t, int i);

    bool is_known();
    void set_known();

    bool check(void * what);
    bool check_class(Class_id id);
    int get_id();
    size_t get_size();
};

class ElementsListIterator
{
    ListElement * le;

  public:
    ElementsListIterator(ListElement * le);
    bool operator!=(const ElementsListIterator & other);
    ElementsListIterator operator++();
    InventoryElement * operator*();

    ElementsListIterator next();
    bool equal(const ElementsListIterator & other);
    InventoryElement * get();
};

class ElementsListReverseIterator
{
    ListElement * le;

  public:
    ElementsListReverseIterator(ListElement * le);
    bool operator!=(const ElementsListReverseIterator & other) const;

    ElementsListReverseIterator & operator++();

    InventoryElement * operator*() const;
};

class ReversedView;
typedef bool (*FindFunc)(NetworkObject * el, void * arg);
class ElementsList
{
    NetworkObject ** find_by_fun(FindFunc fun, void * arg, int * count);

  protected:
    void virtual copy(ListElement * el)
    {
    }

  public:
    void remove_all();
    const char * name;
    int nr_elements;
    ListElement * head;
    ListElement * tail;
    ElementsList(const char * n);
    ElementsList();
    virtual ~ElementsList();
    ListElement * find(void * what);
    bool virtual find_check(ListElement * el, void * what);
    void show(bool details = true);
    void enable_all();
    void disable_all();
    ListElement * add(ListElement * el);
    ListElement * add_front(ListElement * el);
    InventoryElement * add(InventoryElement * el);
    InventoryElement * add_front(InventoryElement * el);

    NetworkObject ** find_form(enum Form f, int * count);
    NetworkObject ** find_class(enum Class_id cl, int * count);
    NetworkObject ** find_id(size_t id, int * count);
    //  bool virtual find_at_check(ListElement *el, void * pos) { return false; }

    void remove(InventoryElement * el);
    void remove(ListElement * el);
    void tick();
    void copy_elements(ElementsList * dst);
    ListElement * get_random();

    ElementsListIterator begin() const;
    ElementsListIterator end() const;

    ElementsListReverseIterator rbegin();
    ElementsListReverseIterator rend();

    ReversedView reversed();
};

class ReversedView
{
  public:
    ElementsList * list;
    ReversedView(ElementsList * list);
    void show();
    ElementsListReverseIterator begin();
    ElementsListReverseIterator end();
};

extern ElementsList base_elements;
extern ElementsList base_plants;
extern ElementsList base_animals;

#endif // EL_LIST_H
