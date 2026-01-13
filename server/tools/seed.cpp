#include "tools.h"

Seed::Seed(InventoryElement * from) : IngredientServer(from, ING_SEED, Form_solid)
{
}

IngredientServer * Seed::createSeed(InventoryElement * from)
{
    IngredientServer * ret = nullptr;

    switch (from->get_cid())
    {
        case Class_Plant:
            if (dynamic_cast<Plant *>(from)->phase == Plant_fruits)
                ret = new Seed(from);
            break;
        case Class_Ingredient:
            if (from->get_id() == ING_FRUIT)
                ret = new Seed(from);
            break;
    }

    return ret;
}
