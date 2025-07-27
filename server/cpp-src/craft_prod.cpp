#include "craft_prod.h"

typedef ProductServer * (*ProductFunction)(InventoryElement *, InventoryElement *);

ProductFunction productFunctions[] = {createAxe, createPickAxe, createHut, createKnife, createFire, createRoasted_meat};

InventoryElement * craft_prod(int product_id, InventoryElement * el1, InventoryElement * el2)
{
    if (product_id >= PROD_COUNT)
        return nullptr;
    InventoryElement * crafted = productFunctions[product_id](el1, el2);

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
