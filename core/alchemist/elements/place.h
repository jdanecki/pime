#ifndef __ELEMENTS_PLACE_H
#define __ELEMENTS_PLACE_H

#include "inventory_element.h"

class Place : public InventoryElement
{
  public:
    Place_id id;

    const char * get_name() override;
    Place(Place_id id, size_t uid);
    Place(Place_id id);
    void show(bool details = true) override;
    char * get_description() override;
    virtual void show_state();
    size_t get_id() override;
};

#endif
