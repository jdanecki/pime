#include "tools.h"
#include "../../core/world.h"

Feed::Feed(InventoryElement * el1, InventoryElement * el2) : ProductServer(el1, el2, PROD_FEED, Form_solid, 1)
{
    actions[0] = ACT_FEED;
}

ProductServer * Feed::createFeed(InventoryElement * el1, InventoryElement * el2)
{
    int id1 = el1->get_id();
    int id2 = el2->get_id();

    if ((id1 == ING_MEAT && id2 == ING_TINDER) || (id1 == ING_TINDER && id2 == ING_MEAT))
        return new Feed(el1, el2);
    CONSOLE_LOG(" wrong ingredients id1=%d id=%d\n", id1, id2);
    return nullptr;
}

bool Feed::use_on(InventoryElement * object, Player * pl)
{
    if (object->c_id == Class_Animal)
    {
        CONSOLE_LOG("feeding %s\n", object->get_name());
        AnimalServer * animal = static_cast<AnimalServer *>(object);
        return animal->feed();
    }
    return false;
}