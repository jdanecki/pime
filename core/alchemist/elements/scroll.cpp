#include "scroll.h"
#include <cstdio>

Base * Scroll::get_base()
{
    return base.get();
}
Class_id Scroll::get_base_cid()
{
    return get_base()->c_id;
}
char * Scroll::get_description()
{
    char * buf = new char[128];
    sprintf(buf, "%s describing %s", get_class_name(), get_name());
    return buf;
}
const char * Scroll::get_name()
{
    return get_base()->get_name();
}
size_t Scroll::get_id()
{
    return get_base()->uid;
}
void Scroll::show(bool details)
{
    InventoryElement::show(details);
    if (details)
        get_base()->show(details);
}
Scroll::Scroll(Base * b) : InventoryElement(Class_Scroll), base(b)
{
}
