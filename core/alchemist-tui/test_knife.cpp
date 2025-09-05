#include "test_knife.h"
#include <stdlib.h>

ProductServer * test_knife()
{
    Element * element1 = new Element(new BaseElement(Form_solid, 0));
    Element * element2 = new Element(new BaseElement(Form_solid, 0));
    printf("element1 = %s\n", element1->get_name());
    printf("element2 = %s\n", element2->get_name());

    IngredientServer * knife_blade = KnifeBlade::createKnifeBlade(element1);
    if (!knife_blade)
        return nullptr;
    //    knife_blade->show();

    IngredientServer * knife_handle = KnifeHandle::createKnifeHandle(element2);
    if (!knife_handle)
        return nullptr;
    //  knife_handle->show();

    ProductServer * knife = Knife::createKnife(knife_blade, knife_handle);
    if (!knife)
        return nullptr;
    knife->show();

    return knife;
}
