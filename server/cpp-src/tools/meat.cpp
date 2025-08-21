#include "tools.h"
#include "../craft_prod.h"

Meat::Meat(InventoryElement * from) : IngredientServer(from, ING_MEAT, Form_solid)
{
}

bool Meat::action(Product_action action, Player * pl)
{
    printf("MEAT: %s %s\n", product_action_name[action], get_name());

    InventoryElement * crafted = nullptr;

    switch (action)
    {
        case ACT_FIRE:
        {
            // TODO FIXME @jacek
            // Fire * fire = new Fire(nullptr, nullptr);
            crafted = craft_prod((int)PROD_ROASTED_MEAT, this, nullptr/*fire*/);
            // delete fire;
            // break;
        }
        default:
            return false;
    }
    return false;
}

IngredientServer * Meat::Meat::createMeat(InventoryElement * from)
{
    if (from->get_cid() == Class_Animal)
        return new Meat(from);
    return nullptr;
}
