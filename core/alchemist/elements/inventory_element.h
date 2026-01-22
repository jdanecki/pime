#ifndef __ELEMENTS_INVENTORY_ELEMENT_H
#define __ELEMENTS_INVENTORY_ELEMENT_H

#include "../object.h"
#include "../item_location.h"
#include "types.h"
#include "../properties.h"

class Player;

class InventoryElement : public NetworkObject
{
  public:
    ItemLocation location;

    InventoryElement(Class_id c_id, size_t uid, ItemLocation location);
    InventoryElement(Class_id c_id, size_t uid);
    InventoryElement(Class_id c_id);
    InventoryElement();
    virtual bool action(Product_action action, Player * pl);
    virtual bool player_action(Player_action action, Player * pl);
    virtual void show(bool details = true);
    virtual bool tick();
    virtual const char * get_name();
    const char * get_class_name();
    virtual size_t get_id();
    virtual Class_id get_base_cid();

    virtual bool check_rect(float px, float py, int size);

    virtual char * get_description();
    virtual Property ** get_properties(int * count);
    virtual ~InventoryElement();
    virtual void update_item_location(ItemLocation &, ItemLocation &);
    virtual Form get_form();
    virtual const char * get_form_name();
    virtual bool can_pickup();
};

#endif
