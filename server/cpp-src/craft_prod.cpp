#include "craft_prod.h"
#include "tools/tools.h"

typedef ProductServer * (*ProductFunction)(InventoryElement *, InventoryElement *);

ProductFunction productFunctions[] = {
    Axe::createAxe,
    Knife::createKnife,
    Pickaxe::createPickaxe,
    Hoe::createHoe,
    Hut::createHut,
    Tent::createTent,
    Fire::createFire,
    RoastedMeat::createRoastedMeat,
    FruitSalad::createFruitSalad,
    Seedling::createSeedling,
};

InventoryElement * craft_prod(int product_id, InventoryElement * el1, InventoryElement * el2)
{
    if (product_id >= PROD_COUNT)
        return nullptr;
    if (el1->c_id == Class_Ingredient && el2->c_id == Class_Ingredient)
        return productFunctions[product_id](el1, el2);
    else
    {
        CONSOLE_LOG(" wrong ingredients class\n");
        return nullptr;
    }
}
