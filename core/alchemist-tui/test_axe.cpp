#include "test_axe.h"
#include <stdlib.h>

ProductServer * test_axe()
{
    Element * element1 = new Element(new BaseElement(Form_solid, 0));
    Element * element2 = new Element(new BaseElement(Form_solid, 0));
    printf("element1 = %s\n", element1->get_name());
    printf("element2 = %s\n", element2->get_name());

    IngredientServer * axe_blade = AxeBlade::createAxeBlade(element1);
    if (!axe_blade)
        return nullptr;
    //    axe_blade->show();

    IngredientServer * axe_handle = AxeHandle::createAxeHandle(element2);
    if (!axe_handle)
        return nullptr;
    //  axe_handle->show();

    ProductServer * axe = Axe::createAxe(axe_blade, axe_handle);
    if (!axe)
        return nullptr;
    axe->show();

    return axe;
}
