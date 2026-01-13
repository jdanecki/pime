#include "craft_ing.h"
#include "tools/tools.h"

typedef IngredientServer * (*IngredientFunction)(InventoryElement *);

IngredientFunction ingredientFunctions[] = {AxeBlade::createAxeBlade, AxeHandle::createAxeHandle, KnifeBlade::createKnifeBlade, KnifeHandle::createKnifeHandle, PickaxeBlade::createPickaxeBlade,
    PickaxeHandle::createPickaxeHandle, HoeBlade::createHoeBlade, HoeHandle::createHoeHandle, Wall::createWall, Meat::createMeat, Log::createLog, Tinder::createTinder, Stick::createStick,
    Fruit::createFruit, Seed::createSeed};

InventoryElement * craft_ing(int ing_id, InventoryElement * el)
{
    if (ing_id >= ING_COUNT)
        return nullptr;
    InventoryElement * crafted = ingredientFunctions[ing_id](el);

    return crafted;
}
