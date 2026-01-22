#include "inventory_element.h"
#include <cstdio>

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
bool InventoryElement::check_rect(unsigned int px, unsigned int py, int)
{
    return (px == location.get_world_x() && py == location.get_world_y());
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
