#include "tools.h"

RoastedMeat::RoastedMeat(InventoryElement * el1, InventoryElement * el2) : ProductServer(el1, el2, PROD_ROASTED_MEAT, Form_solid, 0)
{
    // FIXME
    //    actions = ACT_NOTHING;
}

bool RoastedMeat::player_action(Player_action action, Player * pl)
{
    CONSOLE_LOG("ROASTED_MEAT: %s %s\n", player_action_name[action], get_name());

    switch (action)
    {
        case PLAYER_EAT:
            pl->hunger += this->quality.value * 2 - 40;
            CONSOLE_LOG("ate %s\n", get_name());
            destroy(this);
            break;
    }

    return true;
}

ProductServer * RoastedMeat::createRoastedMeat(InventoryElement * el1, InventoryElement * el2)
{
    int id1 = el1->get_id();
    int id2 = el2->get_id();

    if ((id1 == ING_MEAT && id2 == PROD_FIRE) || (id2 == ING_MEAT && id1 == PROD_FIRE))
        return new RoastedMeat(el1, el2);
    CONSOLE_LOG(" wrong ingredients\n");
    return nullptr;
}
