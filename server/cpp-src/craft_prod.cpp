#include "craft_prod.h"

InventoryElement * craft_prod(int product_id, InventoryElement * el1, InventoryElement *el2)
{
    InventoryElement * crafted=nullptr;

    switch(product_id)
    {
        case PROD_AXE: crafted  = new Axe(el1, el2); break;
        case PROD_PICKAXE: crafted = new PickAxe(el1, el2); break;
        case PROD_HUT: crafted = new Hut(el1, el2); break;
        case PROD_KNIFE: crafted = new Knife(el1, el2); break;
    }

    if (crafted->craft())
    {
        crafted->show();
        return crafted;
    }
    else
    {
        delete crafted;
    }

    return nullptr;
}
