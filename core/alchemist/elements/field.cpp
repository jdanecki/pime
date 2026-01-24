#include "field.h"
#include "alchemist/elements/types.h"

void Field::show_state()
{
    CONSOLE_LOG("Field state: %s\n", field_states_names[state]);
}

Field::Field(Place_id id) : Place(id), InventoryElement(Class_Field)
{
    state = FIELD_PLOWED;
}

Field::Field(Place_id id, size_t uid) : Place(id), InventoryElement(Class_Field, uid)
{
}

void Field::show(bool details)
{
    InventoryElement::show(details);
    Place::show(details);
}

const char * Field::get_name()
{
    return Place::get_place_name();
}

size_t Field::get_id()
{
    return id;
}
