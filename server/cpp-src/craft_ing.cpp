#include "craft_ing.h"

typedef IngredientServer * (*IngredientFunction)(InventoryElement *);

IngredientFunction ingredientFunctions[] = {createAxeBlade, createAxeHandle,

    createKnifeBlade, createKnifeHandle,

    createPickAxeBlade, createPickAxeHandle,

    createWall, createMeat, createLog, createTinder, createStick};

InventoryElement * craft_ing(int product_id, InventoryElement * el)
{
    if (product_id >= ING_COUNT)
        return nullptr;
    InventoryElement * crafted = ingredientFunctions[product_id](el);

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
