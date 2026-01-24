#include "places.h"

FieldServer::FieldServer(Place_id id) : Field(id)
{
}

void FieldServer::show_state()
{
    CONSOLE_LOG("Field state: %s\n", field_states_names[state]);
}

InventoryElement * FieldServer::createField(Place_id id)
{
    return new FieldServer(id);
}

bool FieldServer::can_pickup()
{
    return false;
}

bool BarnServer::can_pickup()
{
    return false;
}

BarnServer::BarnServer(Place_id id) : Barn(id)
{
    state = BARN_ANIMAL;
}

void BarnServer::show_state()
{
    CONSOLE_LOG("Barn state: %s\n", barn_states_names[state]);
}

InventoryElement * BarnServer::createBarn(Place_id id)
{
    return new BarnServer(id);
}
bool FieldServer::action(Product_action action, Player * pl)
{
    CONSOLE_LOG("Field: %s %s\n", product_action_name[action], get_name());
    return false;
}

bool BarnServer::action(Product_action action, Player * pl)
{
    CONSOLE_LOG("Barn: %s %s\n", product_action_name[action], get_name());
    return false;
}

typedef InventoryElement * (*PlaceFunction)(Place_id id);

PlaceFunction PlaceFunctions[] = {
    FieldServer::createField,
    BarnServer::createBarn,
};

InventoryElement * create_place(Place_id id)
{
    if (id >= PLACES_COUNT)
        return nullptr;
    CONSOLE_LOG("create_place %d\n", id);
    return PlaceFunctions[id](id);
}
