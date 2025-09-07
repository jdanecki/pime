#include "el_list.h"
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <cassert>
#include <errno.h>
#include <sys/ioctl.h>
#include <unistd.h>

ListElement::ListElement(InventoryElement * entry)
{
    el = entry;
    next = nullptr;
    prev = nullptr;
    enabled = true;
}

void ListElement::add(ListElement * entry)
{
    next = entry;
    entry->prev = this;
}

void ListElement::show(bool details)
{
    el->show(details);
}

void ElementsList::remove_all()
{
    while(head)
    {
        remove(head);
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
    printf("--- %s (%d) ---\n", name, nr_elements);
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
    }
    nr_elements++;
    return entry;
}

ListElement * ElementsList::add_front(ListElement * entry)
{
    assert(entry);

    if (head)
    {
        entry->next=head;
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
        if (tmp) tmp->prev = nullptr;
        assert(tail);

        if (tail == el) // only 1 element on the list
        {
            if (head == tail)
                tail = NULL;
        }
        free(head);
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
            if (tmp && tmp->next) tmp->next->prev = cur;
            cur->next = cur->next->next;
            if (tail == el)
            {
                tail = cur;
            }
            free(tmp);
            nr_elements--;
            return;
        }
        cur = cur->next;
    }
}

InventoryElement ** InvList::find_by_fun(FindFunc fun, void *arg, int *count)
{
    ListElement * cur = head;
    InventoryElement ** a = (InventoryElement **)calloc(nr_elements, sizeof(InventoryElement *));
    int c = 0;
    while (cur)
    {
        if (fun(cur->el, arg))
        {
            a[c] = cur->el;
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

InventoryElement **InvList::find_class(Class_id cl, int *count)
{
    return find_by_fun(match_class, &cl, count);
}

InventoryElement ** InvList::find_id(int id, int * count)
{
    //FIXME
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
        free(a);
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
  //  printf("%s elements=%d %s\n", name, nr_elements, el->get_class_name());
    return el;
}

InventoryElement * InvList::add_front(InventoryElement * el)
{
    ListElement * entry = new ListElement(el);
    ElementsList::add_front(entry);
    //  printf("%s elements=%d %s\n", name, nr_elements, el->get_class_name());
    return el;
}
void InvList::remove(InventoryElement * el)
{
    if (!head)
        return;
    ListElement * cur = head;
    ListElement * tmp;
    if (head->el == el)
    {
        tmp = head->next;
        if (!tail)
        {
            exit(0);
        }
        if (tail->el == el) // only 1 element on the list
        {
            if (head == tail)
                tail = NULL;
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
        if (cur->next->el == el)
        {
            tmp = cur->next;
            cur->next = cur->next->next;
            if (tail->el == el)
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

void ElementsList::copy_elements(ElementsList * dst)
{
    ListElement * cur = head;
    while(cur) {
        dst->copy(cur);
        cur = cur->next;
    }
}

ListElement *ElementsList::get_random()
{
    if (!nr_elements) return nullptr;
    int i = rand() % nr_elements;
    int cnt=0;
    ListElement * el = head;
    while(el)
    {
        if (i == cnt) return el;
        el=el->next;
        cnt++;
    }
    return nullptr;
}


