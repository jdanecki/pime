#include "craft_ing.h"
#include "tools/tools.h"
#include "ncurses-costam.h"

typedef IngredientServer * (*IngredientFunction)(InventoryElement *);

IngredientFunction ingredientFunctions[] = {AxeBlade::createAxeBlade, AxeHandle::createAxeHandle, KnifeBlade::createKnifeBlade, KnifeHandle::createKnifeHandle, PickaxeBlade::createPickaxeBlade,
    PickaxeHandle::createPickaxeHandle, Wall::createWall, Meat::createMeat, Log::createLog, Tinder::createTinder, Stick::createStick, Fruit::createFruit};

InventoryElement * craft_ing(int product_id, InventoryElement * el)
{
    if (product_id >= ING_COUNT)
        return nullptr;
    InventoryElement * crafted = ingredientFunctions[product_id](el);

    return crafted;
}
