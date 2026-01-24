#include <cstdio>
#include "place.h"
#include "../object.h"

const char * Place::get_place_name()
{
    return places_names[id];
}
void Place::show(bool details)
{
    CONSOLE_LOG("Place type: %s\n", get_place_name());
    show_state();
}
char * Place::get_description()
{
    char * buf = new char[128];
    sprintf(buf, "%s", get_place_name());
    return buf;
}
void Place::show_state()
{
}

Place::Place(Place_id id) : id(id)
{
}
