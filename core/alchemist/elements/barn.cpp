#include "barn.h"
#include "types.h"

void Barn::show_state()
{
    CONSOLE_LOG("Barn state: %s\n", barn_states_names[state]);
}

Barn::Barn(Place_id id) : Place(id), InventoryElement(Class_Barn)
{
    state = BARN_ANIMAL;
}

Barn::Barn(Place_id id, size_t uid) : Place(id), InventoryElement(Class_Barn, uid)
{
}

void Barn::show(bool details)
{
    InventoryElement::show(details);
    Place::show(details);
}

const char * Barn::get_name()
{
    return Place::get_place_name();
}
size_t Barn::get_id()
{
    return id;
}
