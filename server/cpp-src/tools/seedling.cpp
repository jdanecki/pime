#include "tools.h"
#include "../../../core/world.h"

Seedling::Seedling(InventoryElement * el1, InventoryElement * el2) : ProductServer(el1, el2, PROD_SEEDLING, Form_solid, 1)
{
    actions[0] = ACT_PLANT;
}

ProductServer * Seedling::createSeedling(InventoryElement * el1, InventoryElement * el2)
{    
    int id1 = el1->get_id();
    int id2 = el2->get_id();

    if ((id1 == ING_SEED && id2 == ING_SEED))
        return new Seedling(el1, el2);
    CONSOLE_LOG(" wrong ingredients id1=%d id=%d\n", id1, id2);
    return nullptr;
}

bool Seedling::use(InventoryElement * object, Player * pl)
{
    CONSOLE_LOG("%s: %s on %s\n", get_name(), product_action_name[actions[0]], object->get_name());
    return false;
}
