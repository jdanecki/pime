#include "animal_server.h"
#include "../../core/world_params.h"
#include "../../core/world.h"
#include "../../core/player.h"
#include "../../core/alchemist/random_functions.h"
#include "../craft_ing.h"
#include "../elements_server.h"

AnimalServer::AnimalServer(BaseAnimal * base) : Animal(base)
{
    max_age = new Property("max age", 1000, 2000);
    age = new Property("age", 10, max_age->value);
    dimensions.scale(age->value / max_age->value);
}

bool AnimalServer::action(Product_action action, Player * pl)
{
    // Animal::action(action, pl);
    CONSOLE_LOG("ANIMAL_SERVER: %s %s\n", product_action_name[action], get_name());

    InventoryElement * crafted = nullptr;
    switch (action)
    {
        case ACT_CUT:
        case ACT_HIT:
        case ACT_STAB:
            crafted = craft_ing((int)ING_MEAT, this);
            break;
        default:
            return false;
    }
    if (crafted)
    {
        add_object_to_world(crafted, pl->location);
        //  notify_create(crafted);
        CONSOLE_LOG("crafted meat\n");
        destroy(this);
        return true;
    }
    else
    {
        CONSOLE_LOG("failed to craft\n");
    }
    return false;
}

void AnimalServer::show(bool details)
{
    Animal::show(details);
    BeingServer::show(details);
}

bool AnimalServer::grow()
{
    delay_for_grow--;
    // unsigned long ms=get_time_ms();
    // CONSOLE_LOG("AnimalServer.grow: %llu:%llu ms delay=%d\n", ms/1000, ms % 1000, delay_for_grow);

    if (delay_for_grow)
        return false;
    delay_for_grow = max_delay_grow;

    bool ret = BeingServer::grow();
    if (!alive)
    {
        CONSOLE_LOG("%s is dead age=%d/%d\n", get_name(), age->value, max_age->value);
        destroy(this);
        // FIXME leave meat after death
    }
    else
    {
        dimensions.width.value = 2.0 * age->value / max_age->value;
        dimensions.height.value = 2.0 * age->value / max_age->value;
        dimensions.volume.value = dimensions.width.value * dimensions.height.value * dimensions.length.value;
        notify_update(this);
    }
    return ret;
}

bool AnimalServer::tick()
{
    if (check_move())
    {
        ItemLocation old_location = location;
        //   CONSOLE_LOG("AnimalServer::tick: %s\n", get_name());
        discover(this);
        if (old_location != location)
        {
            update_location(NetworkObject(get_cid(), get_uid()), old_location, location);
        }
        return BeingServer::tick();
    }
    return true;
}

AnimalServer * create_animal(BaseAnimal * base)
{
    return new AnimalServer(base);
}

bool AnimalServer::can_pickup()
{
    return true;
}

bool AnimalServer::feed()
{
    wild = false;
    return true;
}
