#include "el_list.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <errno.h>
#include <sys/ioctl.h>
#include <unistd.h>

ListElement::ListElement(InventoryElement * entry) : el(entry), c_id(Class_ListElement)
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
    el.get()->show(details);
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
    CONSOLE_LOG("--- %s (%d) ---\n", name, nr_elements);
    while (cur)
    {
        cur->show(details);
        cur = cur->next;
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
        entry->next=nullptr;
        entry->prev=nullptr;
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
        entry->prev=nullptr;
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
      //  CONSOLE_LOG("EL: delete %p from %p %s\n", head, this, name);
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
        //    CONSOLE_LOG("EL: delete %p from %p %s\n", tmp, this, name);
            delete tmp;
            nr_elements--;
            return;
        }
        cur = cur->next;
    }
}

InventoryElement ** InvList::find_by_fun(FindFunc fun, void * arg, int * count)
{
    ListElement * cur = head;
    InventoryElement ** a = (InventoryElement **)calloc(nr_elements, sizeof(InventoryElement *));
    int c = 0;
    while (cur)
    {
        if (fun(cur->el.get(), arg))
        {
            a[c] = cur->el.get();
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
}

ReversedView ElementsList::reversed()
{
    return ReversedView(this);
}

bool match_form(InventoryElement * el, void * arg)
{
    enum Form f = *(enum Form *)arg;
    return el->get_form() == f;
}

InventoryElement ** InvList::find_form(enum Form f, int * count)
{
    return find_by_fun(match_form, &f, count);
}

bool match_class(InventoryElement * el, void * arg)
{
    enum Class_id cl = *(enum Class_id *)arg;
    return el->get_cid() == cl;
}

InventoryElement ** InvList::find_class(Class_id cl, int * count)
{
    return find_by_fun(match_class, &cl, count);
}

InventoryElement ** InvList::find_id(int id, int * count)
{
    // FIXME
#if 0
    ListElement * cur = head;
    InventoryElement ** a = (InventoryElement **)calloc(nr_elements, sizeof(InventoryElement *));
    int c = 0;

  /*  while (cur)
    {
        if (cur && cur->el && cur->el->get_base() && cur->el->get_base()->id == id)
        {
            a[c] = cur->el;
            c++;
        }
        cur = cur->next;
    }
*/
    if (!c)
    {
        delete a;
        return NULL;
    }
    else
    {
        if (count)
            *count = c;
        return a;
    }
#endif
    return nullptr;
}

InventoryElement * InvList::add(InventoryElement * el)
{
    ListElement * entry = new ListElement(el);
    ElementsList::add(entry);
   // CONSOLE_LOG("InvList: added to list(%s) elements=%d el_class_name:%s\n", name, nr_elements, el->get_class_name());
    return el;
}

InventoryElement * InvList::add_front(InventoryElement * el)
{
    ListElement * entry = new ListElement(el);
    ElementsList::add_front(entry);
    // CONSOLE_LOG("%s elements=%d %s\n", name, nr_elements, el->get_class_name());
    return el;
}
void InvList::remove(InventoryElement * el)
{
    assert(head);
    ListElement * cur = head;
    ListElement * tmp;
    if (head->el.get() == el)
    {
        tmp = head->next;
        if (tmp)
            tmp->prev = nullptr;
        assert(tail);

        if (tail->el.get() == el) // only 1 element on the list
        {           
            tail = nullptr;
        }
     //   CONSOLE_LOG("Inv: delete %p from %p %s\n", head, this, name);
        delete head;
        nr_elements--;
        head = tmp;
        return;
    }
    while (cur) // more then 1 element on the list
    {
        if (!cur->next)
            break;
        if (cur->next->el.get() == el)
        {
            tmp = cur->next;
            if (tmp && tmp->next)
                tmp->next->prev = cur;
            cur->next = cur->next->next;
            if (tail->el.get() == el)
            {
                tail = cur;
            }
        //    CONSOLE_LOG("Inv: delete %p from %p %s\n", tmp, this, name);
            delete tmp;
            nr_elements--;
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
    int i = rand() % nr_elements;
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
    base->show(details);
}
