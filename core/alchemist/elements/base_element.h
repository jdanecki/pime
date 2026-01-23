#ifndef __ELEMENTS_BASE_ELEMENT_H
#define __ELEMENTS_BASE_ELEMENT_H

#include "base.h"
#include "../properties.h"
#include "types.h"

class BaseElement : public Base
{
  public:
    Form form;      // solid, liquid, gas
    ColorRGB color; // color for tile
    Property density;
    Solid solid;

    BaseElement(Form f, int index);
    void show(bool details = true) override;
    virtual size_t get_size() override;
    Form get_form() override;
    void copy_data(unsigned char * ptr, int i) override;
};

#endif
