#include "plant_server.h"
#include "../elements_server.h"
#include "../../core/player.h"
#include "../../core/time_core.h"

PlantServer::PlantServer(BasePlant * base) : Plant(base)
{
    max_age = new Property("max age", flowers_time + 50);
    age = new Property("age", 1);
    set_phase(phase);
}

void PlantServer::change_phase(Plant_phase p)
{
    if (phase != p)
    {
        // CONSOLE_LOG("%s changing phase: %s -> %s age=%u/%u\n", get_name(), plant_phase_name[phase], plant_phase_name[p], age->value, max_age->value);
        notify_update(this);
    }
    phase = p;
}

void PlantServer::show(bool details)
{
    Plant::show(details);
    BeingServer::show(details);
}

bool PlantServer::player_action(Player_action action, Player * pl)
{
    bool res = false;
    // CONSOLE_LOG("PLANT_SERVER: %s %s\n", player_action_name[action], get_name());
    switch (action)
    {
        case PLAYER_EAT:
            CONSOLE_LOG("ate %s\n", get_name());
            res = true;
            if (res)
            {
                pl->hunger += 10;
                destroy(this);
            }
            break;
    }

    return res;
}

void PlantServer::set_phase(Plant_phase p)
{
    phase = p;
    switch (phase)
    {
        case Plant_seedling:
            age->value = seedling_time;
            break;
        case Plant_growing:
            age->value = growing_time;
            break;
        case Plant_flowers:
            age->value = flowers_time;
            break;
        case Plant_fruits:
            age->value = max_age->value;
            grown = true;
            break;
    }
    dimensions.scale(age->value / max_age->value);
    notify_update(this);
}

bool PlantServer::grow()
{
    unsigned long ms = get_time_ms();
    //  CONSOLE_LOG("PlantServer.grow: %llu:%llu ms delay=%d\n", ms/1000, ms % 1000, delay_for_grow);
    if (grown)
        return false;

    delay_for_grow--;
    if (delay_for_grow)
        return false;
    delay_for_grow = max_delay_grow;

    // if (!water)
    //   return !grown;
    // water--;

    age->value++;
    dimensions.scale(age->value / max_age->value);
    notify_update(this);
    /*CONSOLE_LOG("PlantServer:%s growing %d/%d phase=%s grown=%d times=%d/%d/%d/ size=%f\n", get_name(),
        age->value, max_age->value, plant_phase_name[phase], grown,
        seedling_time, growing_time, flowers_time, size);
*/
    if (age->value >= max_age->value)
    {
        if (phase != Plant_fruits)
        {
            grown = true;
            change_phase(Plant_fruits);
        }
        return !grown;
    }
    if (age->value >= flowers_time)
    {
        if (phase != Plant_flowers)
        {
            change_phase(Plant_flowers);
        }
        return !grown;
    }
    if (age->value >= growing_time)
    {
        if (phase != Plant_growing)
        {
            change_phase(Plant_growing);
        }

        return !grown;
    }
    if (age->value >= seedling_time)
    {
        if (phase != Plant_seedling)
        {
            change_phase(Plant_seedling);
        }
        return !grown;
    }
    return !grown;
}

PlantServer * create_plant(BasePlant * base)
{
    return new PlantServer(base);
}

void PlantServer::sow()
{
    change_phase(Plant_seedling);
}
bool PlantServer::action(Product_action action, Player * pl)
{
    Plant::action(action, pl);
    CONSOLE_LOG("PLANT_SERVER: %s %s\n", product_action_name[action], get_name());
    return false;
}
bool PlantServer::can_pickup()
{
    return true;
}