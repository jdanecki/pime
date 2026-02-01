#include "animal_server.h"
#include "../../core/world_params.h"
#include "../../core/world.h"
#include "../../core/player.h"
#include "../../core/alchemist/random_functions.h"
#include "../craft_ing.h"
#include "../elements_server.h"

AnimalServer::AnimalServer(BaseAnimal * base) : Animal(base)
{
    delay_for_move = max_delay_move;
    delay_for_grow = max_delay_grow;
    dst_loc_x = random_range(0, CHUNK_SIZE - 1);
    dst_loc_y = random_range(0, CHUNK_SIZE - 1);
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

void AnimalServer::move()
{
    delay_for_move--;
    if (delay_for_move)
        return;
    delay_for_move = max_delay_move;

    ItemLocation l = location;
    int _x = location.chunk.x;
    int _y = location.chunk.y;

    if ((_x == dst_loc_x && _y == dst_loc_y) || (random_range(0, 5) == 1))
    {
        dst_loc_x = random_range(0, CHUNK_SIZE - 1);
        dst_loc_y = random_range(0, CHUNK_SIZE - 1);
    }
    else
    {
        if (random_range(0, 1))
        {
            if (_x < dst_loc_x)
                _x++;
            else
                _x--;
        }
        if (random_range(0, 1))
        {
            if (_y < dst_loc_y)
                _y++;
            else
                _y--;
        }
    }
    // CONSOLE_LOG("%d, %d -> dst[%d, %d]\n", _x, _y, dst_loc_x, dst_loc_y);

    if (_x >= CHUNK_SIZE)
        _x = CHUNK_SIZE - 1;
    if (_y >= CHUNK_SIZE)
        _y = CHUNK_SIZE - 1;
    if (_y < 0)
        _y = 0;
    if (_y < 0)
        _y = 0;
    if (_x < 0)
        _x = 0;

    location.chunk.x = _x;
    location.chunk.y = _y;

    update_location(NetworkObject(c_id, uid), l, location);
}

bool AnimalServer::tick()
{
    move();
    return BeingServer::tick();
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
