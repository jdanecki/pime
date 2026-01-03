#include "el_list.h"

BasePlant * get_base_plant(size_t id)
{
    printf("get_base_plant id=%ld\n", id);
    return nullptr;
}
BaseAnimal * get_base_animal(size_t id)
{
    printf("get_base_animal id=%ld\n", id);
    return nullptr;
}

BaseElement * get_base_element(size_t id)
{
    printf("get_base_element id=%ld\n", id);
    return nullptr;
}

void test_loops()
{
    ElementsList list("test-list");
    ListElement *el1=new ListElement(new InventoryElement(Class_ListElement));
    list.add_front(el1);

    ListElement *el2=new ListElement(new Place(PLACE_FIELD));
    list.add_front(el2);

    ListElement *el3=new ListElement(new Element(new BaseElement(Form_solid, 1)));
    list.add_front(el3);

    list.show(true);
#if 0
    printf("--- range based for loop\n");
    for (InventoryElement * el : list)
    {
        el->show(false);
    }

    printf("--- range based for loop reversed\n");
    for (InventoryElement * el : list.reversed())
    {
        el->show(false);
    }
#endif

#if 0
    printf("--- for loop with iterator\n");
    for (ElementsListIterator it = list.begin(); it != list.end(); ++it)
    {
        (*it)->show();
    }

    printf("--- for loop with auto\n");
    for (auto it = list.begin(); it != list.end(); ++it)
    {
        (*it)->show();
    }

    printf("--- for loop with reverse iterator\n");
    for (ElementsListReverseIterator it = list.rbegin(); it != list.rend(); ++it)
    {
        (*it)->show();
    }
#endif
}

void test_element_on_2_lists()
{
    ElementsList *list1=new ElementsList("list1");
    ElementsList *list2=new ElementsList("list2");
    InventoryElement *el=new InventoryElement(Class_ListElement);

    for (int i=0; i< 5; i++)
    {
        ListElement *le1=new ListElement(el);
        ListElement *le2=new ListElement(el);
        printf("le1=%p le2=%p\n", le1, le2);
        list1->add_front(le1);
        list2->add_front(le2);
    }

    list1->show();
    list2->show();

    delete list1;
    delete list2;
}

void test_element_on_2_Invlists()
{
    InvList *list1=new InvList("list1");
    InvList *list2=new InvList("list2");

    for (int i=0; i< 5; i++)
    {
        InventoryElement *el=new InventoryElement(Class_Element);
        printf("el=%p\n", el);
        list1->add_front(el);
        list2->add_front(el);
    }

    list1->show();
    list2->show();

    printf("list1->reversed\n");
    for (InventoryElement * el : list1->reversed())
    {
        el->show(false);
    }
    printf("list2->reversed\n");
    for (InventoryElement * el : list2->reversed())
    {
        el->show(false);
    }
    delete list1;
    delete list2;
}

int main()
{
    test_loops();
    //test_element_on_2_lists();
//    test_element_on_2_Invlists();

    return 0;

}
