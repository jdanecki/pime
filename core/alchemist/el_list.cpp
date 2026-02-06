#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <sys/ioctl.h>
#include <unistd.h>

#include "el_list.h"
#include "random_functions.h"

ElementsList base_elements("base elements");
ElementsList base_plants("base plants");
ElementsList base_animals("base animals");

ListElement::ListElement() : c_id(Class_ListElement), el(nullptr), next(nullptr), prev(nullptr)
{
    enable();
}
ListElement::ListElement(NetworkObject * entry) : el(entry), c_id(Class_ListElement)
{
    next = nullptr;
    prev = nullptr;
    enabled = true;
}

void ListElement::add(ListElement * entry)
{
    next = entry;
    entry->prev = this;
    entry->next = nullptr;
}

void ListElement::show(bool details)
{
    //  CONSOLE_LOG("ListElement: %p next=%p prev=%p\n", this, next, prev);
    (static_cast<InventoryElement *>(el.get()))->show(details);
}

void ElementsList::remove_all()
{
    while (head)
    {
        //        CONSOLE_LOG("removing %p elements=%d\n", head, nr_elements);
        remove(head);
        //      CONSOLE_LOG("removed: head=%p tail=%p elements=%d\n", head, tail, nr_elements);
    }
}

ElementsListIterator::ElementsListIterator(ListElement * le) : le(le){};

bool ElementsListIterator::operator!=(const ElementsListIterator & other)
{
    return le != other.le;
}

ElementsListIterator ElementsListIterator::operator++()
{
    le = le->next;
    return *this;
}

InventoryElement * ElementsListIterator::operator*()
{
    return (InventoryElement *)le->get_el();
}

bool ElementsListIterator::equal(const ElementsListIterator & other)
{
    return (*this != other);
}

ElementsListIterator ElementsListIterator::next()
{
    return ++*this;
}

InventoryElement * ElementsListIterator::get()
{
    return **this;
}

ElementsListIterator ElementsList::begin() const
{
    return head;
}
ElementsListIterator ElementsList::end() const
{
    return nullptr;
}

ElementsList::ElementsList(const char * n)
{
    name = n;
    nr_elements = 0;
    tail = NULL;
    head = NULL;
}

ElementsList::ElementsList()
{
    name = "Elements list";
    nr_elements = 0;
    tail = NULL;
    head = NULL;
}

ListElement * ElementsList::find(void * what)
{
    ListElement * cur = head;
    while (cur)
    {
        if (find_check(cur, what))
            return cur;
        cur = cur->next;
    }
    return NULL;
}

void ElementsList::show(bool details)
{
    ListElement * cur = head;
    int i = 1;
    while (cur)
    {
        CONSOLE_LOG("--- %s (%d/%d) ---\n", name, i, nr_elements);
        cur->show(details);
        cur = cur->next;
        i++;
    }
}

void ElementsList::enable_all()
{
    ListElement * cur = head;

    while (cur)
    {
        cur->enable();
        cur = cur->next;
    }
}

void ElementsList::disable_all()
{
    ListElement * cur = head;

    while (cur)
    {
        cur->disable();
        cur = cur->next;
    }
}

void ElementsList::tick()
{
    ListElement * cur = head;

    while (cur)
    {
        ListElement * next = cur->next;
        bool alive = cur->tick();
        if (!alive)
        {
            remove(cur);
        }
        cur = next;
    }
}

ListElement * ElementsList::add(ListElement * entry)
{
    assert(entry);

    if (nr_elements)
    {
        tail->add(entry);
        tail = entry;
    }
    else
    {
        head = entry;
        tail = entry;
        entry->next = nullptr;
        entry->prev = nullptr;
    }
    nr_elements++;
    return entry;
}

ListElement * ElementsList::add_front(ListElement * entry)
{
    assert(entry);

    if (head)
    {
        entry->next = head;
        entry->prev = nullptr;
        head->prev = entry;
        head = entry;
    }
    else
    {
        head = entry;
        tail = entry;
    }

    nr_elements++;
    return entry;
}

void ElementsList::remove(ListElement * el)
{
    assert(head);

    ListElement * cur = head;
    ListElement * tmp;
    if (head == el)
    {
        tmp = head->next;
        if (tmp)
            tmp->prev = nullptr;
        assert(tail);

        if (tail == el) // only 1 element on the list
        {
            tail = nullptr;
        }
        delete head;
        nr_elements--;
        head = tmp;
        return;
    }
    while (cur) // more then 1 element on the list
    {
        if (!cur->next)
            break;
        if (cur->next == el)
        {
            tmp = cur->next;
            if (tmp && tmp->next)
                tmp->next->prev = cur;
            cur->next = cur->next->next;
            if (tail == el)
            {
                tail = cur;
            }
            delete tmp;
            nr_elements--;
            return;
        }
        cur = cur->next;
    }
}

NetworkObject ** ElementsList::find_by_fun(FindFunc fun, void * arg, int * count)
{
    ListElement * cur = head;
    NetworkObject ** a = (NetworkObject **)calloc(nr_elements, sizeof(NetworkObject *));
    int c = 0;
    while (cur)
    {
        NetworkObject * el = cur->get_el();
        if (fun(el, arg))
        {
            a[c] = el;
            c++;
        }
        cur = cur->next;
    }
    if (!c)
    {
        free(a);
        return NULL;
    }
    else
    {
        *count = c;
        return a;
    }
    *count = 0;
    return nullptr;
}

ReversedView ElementsList::reversed()
{
    return ReversedView(this);
}

bool match_form(NetworkObject * el, void * arg)
{
    enum Form f = *(enum Form *)arg;
    return el->get_form() == f;
}

NetworkObject ** ElementsList::find_form(enum Form f, int * count)
{
    return find_by_fun(match_form, &f, count);
}

bool match_class(NetworkObject * el, void * arg)
{
    enum Class_id cl = *(enum Class_id *)arg;
    return el->get_cid() == cl;
}

NetworkObject ** ElementsList::find_class(Class_id cl, int * count)
{
    return find_by_fun(match_class, &cl, count);
}

bool match_uid(NetworkObject * el, void * arg)
{
    size_t uid = *(size_t *)arg;
    return el->get_uid() == uid;
}

NetworkObject ** ElementsList::find_id(size_t id, int * count)
{
    return find_by_fun(match_uid, &id, count);
}

InventoryElement * ElementsList::add(InventoryElement * el)
{
    ListElement * entry = new ListElement(el);
    add(entry);
    // CONSOLE_LOG("ElementsList: added to list(%s) elements=%d el_class_name:%s\n", name, nr_elements, el->get_class_name());
    return el;
}

InventoryElement * ElementsList::add_front(InventoryElement * el)
{
    ListElement * entry = new ListElement(el);
    add_front(entry);
    // CONSOLE_LOG("%s elements=%d %s\n", name, nr_elements, el->get_class_name());
    return el;
}
void ElementsList::remove(InventoryElement * el)
{
    assert(head);
    ListElement * cur = head;
    ListElement * tmp;
    if (head->get_el() == el)
    {
        remove(head);
        return;
    }
    while (cur) // more then 1 element on the list
    {
        if (!cur->next)
            break;
        if (cur->next->get_el() == el)
        {
            remove(cur->next);
            return;
        }
        cur = cur->next;
    }
}

void ElementsList::copy_elements(ElementsList * dst)
{
    ListElement * cur = head;
    while (cur)
    {
        dst->copy(cur);
        cur = cur->next;
    }
}

ListElement * ElementsList::get_random()
{
    if (!nr_elements)
        return nullptr;
    int i = random_range(0, nr_elements);
    int cnt = 0;
    ListElement * el = head;
    while (el)
    {
        if (i == cnt)
            return el;
        el = el->next;
        cnt++;
    }
    return nullptr;
}

void BaseListElement::show(bool details)
{
    get_el()->show(details);
}
void ListElement::disable()
{
    enabled = false;
}
void ListElement::enable()
{
    enabled = true;
}
bool ListElement::is_enabled()
{
    return enabled;
}
bool ListElement::tick()
{
    return ((InventoryElement *)(el.get()))->tick();
}
bool ListElement::check(void * what)
{
    return what == this;
}
ListElement::~ListElement()
{
}
size_t ListElement::get_size()
{
    return sizeof(ListElement);
}
Class_id ListElement::get_cid()
{
    return c_id;
}
NetworkObject * ListElement::get_el()
{
    return el.get();
}
BaseListElement::BaseListElement(Base * base) : ListElement(base)
{
    c_id = Class_BaseListElement;
}
bool BaseListElement::check(void * what)
{
    size_t * uid = static_cast<size_t *>(what);
    return (*uid == get_el()->get_uid());
}
size_t BaseListElement::get_size()
{
    Base * b = static_cast<Base *>(get_el());
    return b->get_size();
}
KnownElement::KnownElement(Class_id t, int i)
{
    elid.c_id = t;
    elid.id = i;
    known = false;
    c_id = Class_KnownElement;
}
bool KnownElement::is_known()
{
    return known;
}
void KnownElement::set_known()
{
    known = true;
    CONSOLE_LOG("learning %s %d\n", class_name[elid.c_id], elid.id);
}
bool KnownElement::check(void * what)
{
    ElId * i = (ElId *)what;
    return (elid.c_id == i->c_id && elid.id == i->id);
}
bool KnownElement::check_class(Class_id id)
{
    return elid.c_id == id;
}
int KnownElement::get_id()
{
    return elid.id;
}
size_t KnownElement::get_size()
{
    return sizeof(KnownElement);
}
ElementsListReverseIterator::ElementsListReverseIterator(ListElement * le) : le(le)
{
}
bool ElementsListReverseIterator::operator!=(const ElementsListReverseIterator & other) const
{
    return le != other.le;
}
ElementsListReverseIterator & ElementsListReverseIterator::operator++()
{
    le = le->prev;
    return *this;
}
InventoryElement * ElementsListReverseIterator::operator*() const
{
    return (InventoryElement *)le->get_el();
}
ElementsList::~ElementsList()
{
    CONSOLE_LOG("~ElementsList %s\n", name);
    remove_all();
}
bool ElementsList::find_check(ListElement * el, void * what)
{
    return el->check(what);
}
ElementsListReverseIterator ElementsList::rbegin()
{
    return tail;
}
ElementsListReverseIterator ElementsList::rend()
{
    return nullptr;
}
ReversedView::ReversedView(ElementsList * list) : list(list)
{
}
void ReversedView::show()
{
    list->show(false);
}
ElementsListReverseIterator ReversedView::begin()
{
    return list->rbegin();
}
ElementsListReverseIterator ReversedView::end()
{
    return list->rend();
}
