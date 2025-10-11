#include "tools.h"

FruitSalad::FruitSalad(InventoryElement * el1, InventoryElement * el2) : ProductServer(el1, el2, PROD_FRUIT_SALAD, Form_solid)
{
    actions = ACT_NOTHING;
}

ProductServer * FruitSalad::createFruitSalad(InventoryElement * el1, InventoryElement * el2)
{

    int id1 = el1->get_id();
    int id2 = el2->get_id();

    if (id1 == ING_FRUIT && id2 == ING_FRUIT)
        return new FruitSalad(el1, el2);
    CONSOLE_LOG(" wrong ingredients\n");
    return nullptr;
}

bool FruitSalad::player_action(Player_action action, Player * pl)
{
    CONSOLE_LOG("FRUIT_SALAD: %s %s\n", player_action_name[action], get_name());

    switch (action)
    {
        case PLAYER_EAT:
            pl->hunger += this->quality.value * 3 - 100;
            CONSOLE_LOG("ate %s\n", get_name());
            destroy(this);
            break;
    }

    return true;
}
