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


bool Hoe::use(InventoryElement * object, Player * pl)
{
    unsigned int map_x, map_y;
    unsigned int x, y;

    map_x = object->location.chunk.map_x;
    map_y = object->location.chunk.map_y;
    x = object->location.get_x();
    y = object->location.get_y();

    if (object->c_id != Class_Place)
    {
        CONSOLE_LOG("It's not a place to plow\n");
        return false;
    }
    Place * p = (Place *)object;
    if (p->get_id() != PLACE_FIELD)
    {
        CONSOLE_LOG("It's not a field\n");
        return false;
    }
    if (p->state != FIELD_PLOWED)
    {
        p->state = FIELD_PLOWED;
        CONSOLE_LOG("%s: %s on %s @[%d,%d][%d,%d]\n", get_name(), product_action_name[actions[0]],
                object->get_name(), map_x, map_y, x, y);
        notify_update(p);
        return true;
    }
    CONSOLE_LOG("%s: already plowed\n", get_name());
    return false;
}

Hoe * create_hoe()
{
    int count;
    NetworkObject ** base_solid = base_elements.find_form(Form_solid, &count);
    CONSOLE_LOG("count=%d \n", count);
    if (count)
    {
        for (int i=0; i < count; i++)
        {
            BaseElement *base=static_cast<BaseElement*>(base_solid[i]);
            CONSOLE_LOG("solid: %d/%d\n", i, count);
//            base->show(false);
        }

        ElementServer *el1=create_element(static_cast<BaseElement *>(base_solid[0]));
        ElementServer *el2=create_element(static_cast<BaseElement *>(base_solid[1]));
        IngredientServer *hb=HoeBlade::createHoeBlade(el1);
        IngredientServer *hh=HoeHandle::createHoeHandle(el2);
        return static_cast<Hoe*>(Hoe::createHoe(hb, hh));
    }
    return nullptr;

}
