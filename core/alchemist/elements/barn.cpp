#include "barn.h"
#include "types.h"
#include "../random_functions.h"

void Barn::show_state()
{
    CONSOLE_LOG("Barn state: %s\n", barn_states_names[state]);
}

Barn::Barn(Place_id id) : Place(id), InventoryElement(Class_Barn)
{
    state = BARN_ANIMAL;
    dimensions.max_width = random_float_range(1, 2.0);
    dimensions.max_height = random_float_range(1, 2.0);
    animals=1;
}

Barn::Barn(Place_id id, size_t uid) : Place(id), InventoryElement(Class_Barn, uid)
{

}

void Barn::show(bool details)
{
    InventoryElement::show(details);
    Place::show(details);
    CONSOLE_LOG("Barn: animals=%d\n", animals);

}

const char * Barn::get_name()
{
    return Place::get_place_name();
}
size_t Barn::get_id()
{
    return id;
}
