#include <math.h>
#include "being_server.h"
#include "../../core/alchemist/item_location.h"
#include "../../core/alchemist/random_functions.h"
#include "../../core/world_params.h"

void BeingServer::show(bool details)
{
    if (age)
        age->show();
    if (max_age)
        max_age->show();
}

bool BeingServer::grow()
{
    if (!alive)
    {
        return false;
    }
    age->value++;

    if (age->value >= max_age->value)
    {
        alive = false;
    }
    return alive;
}

BeingServer::BeingServer()
{
    delay_for_grow = max_delay_grow;
    dst_loc_x = random_range(0, CHUNK_SIZE);
    dst_loc_y = random_range(0, CHUNK_SIZE);
    max_age = nullptr;
    age = nullptr;
    alive = true;
    delay_for_move = max_delay_move;
}
Property ** BeingServer::get_properties(int * count)
{
    Property ** props = new Property *[2];
    props[0] = age;
    props[1] = max_age;

    *count = 2;
    return props;
}
BeingServer::~BeingServer()
{
    delete age;
    delete max_age;
}
bool BeingServer::tick()
{
    return grow();
}
bool BeingServer::check_move()
{
    delay_for_move--;
    if (delay_for_move)
        return false;
    delay_for_move = max_delay_move;
    return true;
}

float distance(float x1, float y1, float x2, float y2)
{
    float dx = x1 - x2;
    float dy = y1 - y2;
    return sqrt(dx * dx + dy * dy);
}
void being_move(BeingServer * being, ItemLocation * location)
{
    float _x = location->chunk.x;
    float _y = location->chunk.y;

    if ((distance(_x, _y, being->dst_loc_x, being->dst_loc_y) < 2.0) || (random_range(0, 5) == 1))
    {
        being->dst_loc_x = random_range(0, CHUNK_SIZE);
        being->dst_loc_y = random_range(0, CHUNK_SIZE);
    }
    else
    {
        //   if (random_range(0, 2))
        {
            if (_x < being->dst_loc_x)
                _x += 0.1;
            else
                _x -= 0.1;
        }
        //  if (random_range(0, 2))
        {
            if (_y < being->dst_loc_y)
                _y += 0.1;
            else
                _y -= 0.1;
        }
    }

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
    // location->show();
    //   CONSOLE_LOG("-> dst[%f, %f]\n", being->dst_loc_x, being->dst_loc_y);
    location->chunk.x = _x;
    location->chunk.y = _y;
    // location->show();
}
