#include <cstdio>
#include "inventory_element.h"
#include "world.h"

InventoryElement::InventoryElement(Class_id c_id, size_t uid, ItemLocation location) : NetworkObject(c_id, uid), location(location)
{
}
InventoryElement::InventoryElement(Class_id c_id, size_t uid) : NetworkObject(c_id, uid)
{
}
InventoryElement::InventoryElement(Class_id c_id) : NetworkObject(c_id)
{
}

InventoryElement::InventoryElement()
{
}
bool InventoryElement::action(Product_action action, Player * pl)
{
    CONSOLE_LOG("INV: %s %s\n", product_action_name[action], get_name());
    return false;
}
bool InventoryElement::player_action(Player_action action, Player * pl)
{
    CONSOLE_LOG("INV: %s %s\n", player_action_name[action], get_name());
    return false;
}
void InventoryElement::show(bool details)
{
    CONSOLE_LOG("(INV)%s: uid=%lx id=%ld c_id=%d @[%d,%d][%f,%f]\n", get_class_name(), uid, get_id(), c_id, location.chunk.map_x, location.chunk.map_y, location.chunk.x, location.chunk.y);
    dimensions.show();
}
bool InventoryElement::tick()
{
    return false;
}
const char * InventoryElement::get_name()
{
    return nullptr;
}
const char * InventoryElement::get_class_name()
{
    return class_name[c_id];
}
size_t InventoryElement::get_id()
{
    return -1;
}
Class_id InventoryElement::get_base_cid()
{
    return c_id;
}
bool InventoryElement::check_rect(InventoryElement * pl)
{
    float px = pl->location.get_world_x();
    float py = pl->location.get_world_y();
    float pw = pl->dimensions.width.value;
    float ph = pl->dimensions.height.value;
    float el_x = location.get_world_x();
    float el_y = location.get_world_y();
    float el_w = dimensions.width.value;
    float el_h = dimensions.height.value;
    if (px > el_x + el_w || py > el_y + el_h || px + pw < el_x || py + ph < el_y)
        return false;

    /*  CONSOLE_LOG("INV: check_rect this=%s pl=%s\n", get_class_name(), pl->get_class_name());
        CONSOLE_LOG("element:\n");
        location.show();
        dimensions.show(false);
        CONSOLE_LOG("player:\n");
        pl->location.show();
        pl->dimensions.show(false);*/
    return true;
}
char * InventoryElement::get_description()
{
    char * buf = new char[128];
    sprintf(buf, "%s: (%s)", get_class_name(), get_name());
    return buf;
}
Property ** InventoryElement::get_properties(int * count)
{
    *count = 0;
    return nullptr;
}
InventoryElement::~InventoryElement()
{
}
void InventoryElement::update_item_location(ItemLocation &, ItemLocation &)
{
}
Form InventoryElement::get_form()
{
    return Form_unknown;
}
const char * InventoryElement::get_form_name()
{
    return Form_name[Form_unknown];
}
bool InventoryElement::can_pickup()
{
    return false;
}
