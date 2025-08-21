#include "tools.h"

Roasted_meat::Roasted_meat(InventoryElement * el1, InventoryElement * el2) : ProductServer(el1, el2, PROD_ROASTED_MEAT, Form_solid)
{
    actions = ACT_NOTHING;
}

bool Roasted_meat::player_action(Player_action action, Player * pl)
{
    printf("ROASTED_MEAT: %s %s\n", player_action_name[action], get_name());

    switch (action)
    {
        case PLAYER_EAT:
            pl->hunger += this->quality.value * 2 - 40;
            destroy(this);
            break;
    }

        return new Roasted_meat(el1, el2);
}

bool Roasted_meat::check_ing()
{
    int id1 = ings[0]->get_id();
    int id2 = ings[1]->get_id();

    if ((id1 == ING_MEAT && id2 == PROD_FIRE) || (id2 == ING_MEAT && id1 == PROD_FIRE))
        return new Roasted_meat(el1, el2);
    printf(" wrong ingredients\n");
    return nullptr;
}

ProductServer * createRoasted_meat(InventoryElement * el1, InventoryElement * el2)
{
    return new Roasted_meat(el1, el2);
}
