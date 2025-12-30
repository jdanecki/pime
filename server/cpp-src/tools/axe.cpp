#include "tools.h"

Axe::Axe(InventoryElement * el1, InventoryElement * el2) : ProductServer(el1, el2, PROD_AXE, Form_solid, 1)
{
    actions[0]=ACT_HIT;
}

ProductServer * Axe::Axe::createAxe(InventoryElement * el1, InventoryElement * el2)
{
    int id1 = el1->get_id();
    int id2 = el2->get_id();

    if ((id1 == ING_AXE_BLADE && id2 == ING_AXE_HANDLE) || (id1 == ING_AXE_HANDLE && id2 == ING_AXE_BLADE))
        return new Axe(el1, el2);
    CONSOLE_LOG(" wrong ingredients\n");
    return nullptr;
}
