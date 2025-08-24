#include "meat.h"
#include "../networking.h"
#include "../world_server.h"
#include "../craft_prod.h"

Meat::Meat(InventoryElement * from) : IngredientServer(from, ING_MEAT, Form_solid)
{

}

bool Meat::check_ing()
{
    if (el->get_cid() == Class_Animal)
        return true;
    return false;
}

bool Meat::action(Product_action action, Player * pl)
{
    printf("MEAT: %s %s\n", product_action_name[action], get_name());

    InventoryElement * crafted = nullptr;

    switch (action)
    {
        case ACT_FIRE:
        {
            Fire * fire = new Fire(nullptr, nullptr);
            crafted = craft_prod((int)PROD_ROASTED_MEAT, this, fire);
            delete fire;
            break;
        }
        default:
            return false;
    }
    if (crafted)
    {
        world_table[pl->map_y][pl->map_x]->add_object(crafted, pl->x, pl->y);
        objects_to_create.add(crafted);
        printf("crafted\n");
        destroy(this);
        return true;
    }
    else
    {
        printf("failed to craft\n");
    }
    return false;
}

IngredientServer * createMeat(InventoryElement * from)
{
    return new Meat(from);
}
