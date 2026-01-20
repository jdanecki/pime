#ifndef __ELEMENTS_SCROLL_H
#define __ELEMENTS_SCROLL_H

#include "inventory_element.h"
#include "base.h"
#include "../object.h"

class Scroll : public InventoryElement
{
    SerializablePointer<Base> base;

  public:
    Base * get_base();
    Scroll(Base * b);
    Class_id get_base_cid() override;
    char * get_description() override;
    const char * get_name() override;
    size_t get_id() override;
    void show(bool details) override;
};

#endif
