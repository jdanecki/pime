#include "craft_prod.h"
#include "tools/tools.h"

typedef ProductServer * (*ProductFunction)(InventoryElement *, InventoryElement *);

ProductFunction productFunctions[] = {
    Axe::createAxe,
    Knife::createKnife,
    Pickaxe::createPickaxe,
    Hut::createHut,
    Fire::createFire,
    RoastedMeat::createRoastedMeat,
    FruitSalad::createFruitSalad,
};

InventoryElement * craft_prod(int product_id, InventoryElement * el1, InventoryElement * el2)
{
    if (product_id >= PROD_COUNT)
        return nullptr;
    InventoryElement * crafted = productFunctions[product_id](el1, el2);

    return crafted;
}
