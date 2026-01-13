#include "places.h"

Field::Field(Place_id id) : PlaceServer(id)
{
    state = FIELD_PLOWED;
}

PlaceServer * Field::createField(Place_id id)
{
    return new Field(id);
}

bool Field::action(Product_action action, Player * pl)
{
    CONSOLE_LOG("Field: %s %s\n", product_action_name[action], get_name());
    return false;
}

typedef PlaceServer * (*PlaceFunction)(Place_id id);

PlaceFunction PlaceFunctions[] = {
    Field::createField,
};

PlaceServer * create_place(Place_id id)
{
    if (id >= PLACES_COUNT)
        return nullptr;
    CONSOLE_LOG("create_place %d\n", id);
    return PlaceFunctions[id](id);
}
