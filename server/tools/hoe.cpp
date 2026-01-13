#include "tools.h"
#include "../../core/world.h"
#include "../places/places.h"

Hoe::Hoe(InventoryElement * el1, InventoryElement * el2) : ProductServer(el1, el2, PROD_HOE, Form_solid, 1)
{
    actions[0] = ACT_PLOW;
}

ProductServer * Hoe::createHoe(InventoryElement * el1, InventoryElement * el2)
{
    int id1 = el1->get_id();
    int id2 = el2->get_id();

    if ((id1 == ING_HOE_BLADE && id2 == ING_HOE_HANDLE) || (id1 == ING_HOE_HANDLE && id2 == ING_HOE_BLADE))
        return new Hoe(el1, el2);
    CONSOLE_LOG(" wrong ingredients\n");
    return nullptr;
}

bool Hoe::use_tile(int map_x, int map_y, int x, int y, Player * pl)
{
    bool ret = ProductServer::use_tile(map_x, map_y, x, y, pl);
    if (!ret)
        return ret;

    chunk * ch = world_table[map_y][map_x];
    CONSOLE_LOG("tile: %d\n", ch->table[y][x].tile);
    ch->add_object(create_place(PLACE_FIELD), x, y);
    return true;
}
