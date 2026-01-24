#ifndef __ELEMENTS_FIELD_H
#define __ELEMENTS_FIELD_H

#include "inventory_element.h"
#include "place.h"

class Field : public InventoryElement, public Place
{
  public:
    Field_states state;
    Field(Place_id id);
    Field(Place_id id, size_t uid);
    void show(bool details) override;
    void show_state() override;
    const char * get_name() override;
    size_t get_id() override;
};

#endif