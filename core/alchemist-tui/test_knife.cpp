#include "test_knife.h"
#include <stdlib.h>

Knife * test_knife()
{
    Element * element1 = new Element(new BaseElement(Form_solid,Color {0,0,0}, 0 ));
    Element * element2 = new Element(new BaseElement(Form_solid,Color {0,0,0}, 0 ));
    printf("element1 = %s\n", element1->get_name());
    printf("element2 = %s\n", element2->get_name());

    KnifeBlade * knife_blade = new KnifeBlade(element1);
    if (!knife_blade->craft())
        return nullptr;
    //    knife_blade->show();

    KnifeHandle * knife_handle = new KnifeHandle(element2);
    if (!knife_handle->craft())
        return nullptr;
    //  knife_handle->show();

    Knife * knife = new Knife(knife_blade, knife_handle);
    if (!knife->craft())
        return nullptr;
    knife->show();

    return knife;
}
