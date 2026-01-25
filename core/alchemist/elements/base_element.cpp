#include <cstdio>
#include <cstdlib>

#include "base_element.h"
#include "../random_functions.h"

BaseElement::BaseElement(Form f, int index) : Base(index, Class_BaseElement, create_name(5 - f)), form(f), density("", 0)
{
    // CONSOLE_LOG("BaseElement index=%d name=%s\n", index, get_name());

    switch (form)
    {
        case Form_solid:
            density = Property("density", 50, 2000);
            color.r = random_range(0, 255);
            color.g = random_range(0, 255);
            color.b = random_range(0, 255);
            break;
        case Form_liquid:
            density = Property("density", 500, 2000);
            color.r = random_range(0, 63);
            color.g = random_range(0, 255);
            color.b = 255;
            break;
        case Form_gas:
            density = Property("density", 1);
            edible.set_no_edible();
            int r = random_range(0, 199);
            color.r = random_range(r, r + 55);
            color.g = random_range(r, r + 55);
            color.b = random_range(r, r + 55);
            break;
    }
}

void BaseElement::show(bool details)
{
    Base::show(details);
    CONSOLE_LOG("BaseElement form=%s\n", Form_name[form]);
    if (!details)
        return;
    density.show(); // gęstość
    CONSOLE_LOG("   form = %s color(r:%d g:%d b:%d)\n", Form_name[form], color.r, color.g, color.b);
    switch (form)
    {
        case Form_solid:
            solid.show();
            break;
        default:
            break;
    }
}

size_t BaseElement::get_size()
{
    return sizeof(BaseElement);
}
Form BaseElement::get_form()
{
    return form;
}
void BaseElement::copy_data(unsigned char * ptr, int i)
{
    BaseElement * dst = &((BaseElement *)ptr)[i];
    *dst = *this;
}
