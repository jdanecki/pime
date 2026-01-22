#include "place.h"
#include <cstdio>

const char * Place::get_name()
{
    return places_names[id];
}
void Place::show(bool details)
{
    InventoryElement::show(details);
    CONSOLE_LOG("Place type: %s\n", get_name());
}
char * Place::get_description()
{
    char * buf = new char[128];
    sprintf(buf, "%s", get_name());
    return buf;
}
void Place::show_state()
{
}
size_t Place::get_id()
{
    return (size_t)id;
}
Place::Place(Place_id id, size_t uid) : InventoryElement(Class_Place, uid), id(id)
{
}

Place::Place(Place_id id) : InventoryElement(Class_Place), id(id)
{
}
