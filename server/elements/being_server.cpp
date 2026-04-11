#include <cstdlib>
#include <math.h>
#include "being_server.h"
#include "../world_server.h"
#include "../../core/alchemist/item_location.h"
#include "../../core/alchemist/random_functions.h"
#include "../../core/world_params.h"
#include "../../core/world.h"

void BeingServer::show(bool details)
{
    if (age)
        age->show();
    if (max_age)
        max_age->show();
    CONSOLE_LOG("dst_loc: ");
    dst_loc.show();
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

void BeingServer::discover(InventoryElement * who)
{
    if ((distance(who->location.get_world_x(), who->location.get_world_y(), dst_loc.get_world_x(), dst_loc.get_world_y()) < 2.0)) // || (random_range(0, 5) == 1))
    {
        int dx = random_range(-1, 2);
        int dy = random_range(-1, 2);
        int new_x = who->location.chunk.map_x + dx;
        if (new_x < 0)
            new_x = 0;
        if (new_x == WORLD_SIZE)
            new_x = WORLD_SIZE - 1;
        int new_y = who->location.chunk.map_y + dy;
        if (new_y < 0)
            new_y = 0;
        if (new_y == WORLD_SIZE)
            new_y = WORLD_SIZE - 1;
        dst_loc.set_chunk(new_x, new_y, random_range(0, CHUNK_SIZE), random_range(0, CHUNK_SIZE));
        //  CONSOLE_LOG("discover @ %d,%d - %f,%f\n", dst_loc.chunk.map_x, dst_loc.chunk.map_y, dst_loc.chunk.x, dst_loc.chunk.y);
    }
    move_to(who);
}

void BeingServer::move_to(InventoryElement * who)
{
    float dx = 0;
    float dy = 0;
    dx = dst_loc.get_world_x() - who->location.get_world_x();
    dy = dst_loc.get_world_y() - who->location.get_world_y();
    float lenght = sqrt((dx * dx) + (dy * dy));
    dy /= lenght * 8; // normalize and shorten
    dx /= lenght * 8;
    // add some randomness 25% of the time (per axis)
    if (rand() % 100 < 25)
        dx += (float)(rand() % 3 - 1) / 8;
    if (rand() % 100 > 25)
        dy += (float)(rand() % 3 - 1) / 8;
    move_by(who, dx, dy);
}
void BeingServer::move_by(InventoryElement * who, float dx, float dy)
{
    // CONSOLE_LOG("SERV: being move dx=%f dy=%f\n", dx, dy);
    ItemLocation old = who->location;

    float new_x = who->location.chunk.x + dx;
    float new_y = who->location.chunk.y + dy;
    int new_map_x = who->location.chunk.map_x;
    int new_map_y = who->location.chunk.map_y;

    if (new_x < 0)
    {
        new_map_x = who->location.chunk.map_x - 1;
        new_x += CHUNK_SIZE;
    }
    if (new_y < 0)
    {
        new_map_y = who->location.chunk.map_y - 1;
        new_y += CHUNK_SIZE;
    }
    if (new_x >= CHUNK_SIZE)
    {
        new_map_x++;
        new_x -= CHUNK_SIZE;
    }
    if (new_y >= CHUNK_SIZE)
    {
        new_map_y++;
        new_y -= CHUNK_SIZE;
    }

    if (new_map_x != old.chunk.map_x || new_map_y != old.chunk.map_y)
    {
        check_and_load_chunk(new_map_x, new_map_y);
        remove_from_chunks(who);
        who->location.set_chunk(new_map_x, new_map_y, new_x, new_y);
        add_object_to_world(who);
        return;
    }
    who->location.chunk.x = new_x;
    who->location.chunk.y = new_y;
}
