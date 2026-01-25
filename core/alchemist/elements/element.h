#ifndef __ELEMENTS_ELEMENT_H
#define __ELEMENTS_ELEMENT_H

#include "inventory_element.h"
#include "base_element.h"
#include "../object.h"

class Element : public InventoryElement
{
    SerializablePointer<BaseElement> base;

  public:
    Property sharpness;  // ostrość
    Property smoothness; // gładkość
    Property mass;       // density*volume

    BaseElement * get_base();
    void show(bool details = true) override;
    Element(BaseElement * b);
    Element(int id);
    Form get_form() override;
    const char * get_name() override;
    const char * get_form_name() override;
    size_t get_id() override;
    Class_id get_base_cid() override;

    Property ** get_properties(int * count) override;
    char * get_description() override;
};

BaseElement * get_base_element(size_t id);

#endif
