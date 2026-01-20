#include "element.h"
#include "../el_list.h"
#include <cstdio>
#include <cstdlib>

Element::Element(BaseElement * b)
    : InventoryElement(Class_Element), base(b), length("length", 5 + rand() % 60), width("width", 5 + rand() % 60), height("height", 5 + rand() % 60),
      volume("volume", length.value * width.value * height.value), sharpness("sharpness", 0), smoothness("smoothness", 0), mass("mass", b->density.value * volume.value / 1000)
{
}
Element::Element(int id) : base(get_base_element(id))
{
}

void Element::show(bool details)
{
    InventoryElement::show(details);

    if (!details)
        return;
    CONSOLE_LOG("form=%s\n", get_form_name());
    length.show();
    width.show();
    height.show();
    volume.show();

    if (details)
    {
        sharpness.show();
        smoothness.show();
        mass.show();
    }
    get_base()->show(details);
}

BaseElement * Element::get_base()
{
    return base.get();
}
Form Element::get_form()
{
    return get_base()->form;
}
const char * Element::get_name()
{
    return get_base()->get_name();
}
const char * Element::get_form_name()
{
    return Form_name[get_form()];
}
size_t Element::get_id()
{
    return get_base()->uid;
}
Class_id Element::get_base_cid()
{
    return get_base()->c_id;
}
Property ** Element::get_properties(int * count)
{
    *count = 8;
    Form f = get_form();
    if (f == Form_solid)
        *count += 6;
    Property ** props = new Property *[*count];
    props[0] = &length;
    props[1] = &width;
    props[2] = &height;
    props[3] = &volume;
    props[4] = &sharpness;
    props[5] = &smoothness;
    props[6] = &mass;
    props[7] = &get_base()->density;
    if (f == Form_solid)
    {
        props[8] = &get_base()->solid.tooling;
        props[9] = &get_base()->solid.stretching;
        props[10] = &get_base()->solid.squeezing;
        props[11] = &get_base()->solid.bending;
        props[12] = &get_base()->solid.solubility;
        props[13] = &get_base()->solid.hardness;
    }
    return props;
}
char * Element::get_description()
{
    char * buf = new char[128];
    sprintf(buf, "%s %s: (%s) base=%ld", get_form_name(), get_class_name(), get_name(), get_id());
    return buf;
}

BaseElement * get_base_element(size_t id)
{
    BaseListElement * el = (BaseListElement *)base_elements.find(&id);
    if (!el)
        return nullptr;
    return (BaseElement *)((el)->get_el());
}
