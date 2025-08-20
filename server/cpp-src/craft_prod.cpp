#include "craft_prod.h"

typedef ProductServer * (*ProductFunction)(InventoryElement *, InventoryElement *);

ProductFunction productFunctions[] = {
    Axe::createAxe,
    createKnife,
    createPickAxe,
    createHut,
    createFire,
    createRoasted_meat,
    createFruit_salad,
};

InventoryElement * craft_prod(int product_id, InventoryElement * el1, InventoryElement * el2)
{
    if (product_id >= PROD_COUNT)
        return nullptr;
    InventoryElement * crafted = productFunctions[product_id](el1, el2);

    return crafted;
}
