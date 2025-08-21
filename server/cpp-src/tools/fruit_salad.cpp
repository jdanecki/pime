#include "tools.h"

Fruit_salad::Fruit_salad(InventoryElement * el1, InventoryElement * el2) : ProductServer(el1, el2, PROD_FRUIT_SALAD, Form_solid)
{
    actions = ACT_NOTHING;
}

bool Fruit_salad::check_ing()
{
    int id1 = ings[0]->get_id();
    int id2 = ings[1]->get_id();

    if (id1 == ING_FRUIT && id2 == ING_FRUIT)
        return new Fruit_salad(el1, el2);
    printf(" wrong ingredients\n");
    return nullptr;
}

ProductServer * createFruit_salad(InventoryElement * el1, InventoryElement * el2)
{
    return new Fruit_salad(el1, el2);
}

bool Fruit_salad::player_action(Player_action action, Player * pl)
{
    printf("FRUIT_SALAD: %s %s\n", player_action_name[action], get_name());

    switch (action)
    {
        case PLAYER_EAT:
            pl->hunger += this->quality.value * 3 - 100;
            destroy(this);
            break;
    }

        return new Fruit_salad(el1, el2);
}
