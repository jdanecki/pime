#ifndef BARN_H
#define BARN_H

#include "inventory_element.h"
#include "place.h"

class Barn : public InventoryElement, public Place
{
protected:
	int animals;
  public:
    Barn_states state;
    Barn(Place_id id);
    Barn(Place_id id, size_t uid);
    void show(bool details) override;
    void show_state() override;
    const char * get_name() override;
    size_t get_id() override;
};

#endif
