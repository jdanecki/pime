#include "places.h"

Field::Field(Place_id id) : PlaceServer(id)
{
    state = FIELD_PLOWED;
}

void Field::show_state()
{
    CONSOLE_LOG("Field state: %s\n", field_states_names[state]);
}

PlaceServer * Field::createField(Place_id id)
{
    return new Field(id);
}

Barn::Barn(Place_id id) : PlaceServer(id)
{
    state = BARN_ANIMAL;
}

void Barn::show_state()
{
    CONSOLE_LOG("Barn state: %s\n", barn_states_names[state]);
}

PlaceServer * Barn::createBarn(Place_id id)
{
    return new Barn(id);
}
bool Field::action(Product_action action, Player * pl)
{
    CONSOLE_LOG("Field: %s %s\n", product_action_name[action], get_name());
    return false;
}

bool Barn::action(Product_action action, Player * pl)
{
    CONSOLE_LOG("Barn: %s %s\n", product_action_name[action], get_name());
    return false;
}

typedef PlaceServer * (*PlaceFunction)(Place_id id);

PlaceFunction PlaceFunctions[] = {
    Field::createField,
    Barn::createBarn,
};

PlaceServer * create_place(Place_id id)
{
    if (id >= PLACES_COUNT)
        return nullptr;
    CONSOLE_LOG("create_place %d\n", id);
    return PlaceFunctions[id](id);
}
