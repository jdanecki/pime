#include "elements_server.h"
#include "networking.h"
#include "world_server.h"

#ifndef CORE_TUI
void destroy(InventoryElement * el)
{
    notify_destroy(el->get_uid(), el->location);
    if (el->location.type == LOCATION_CHUNK)
    {
        world_table[el->location.data.chunk.map_y][el->location.data.chunk.map_x]->remove_object(el);
    }
    delete el;
}
#endif

AnimalServer::AnimalServer()
{
    delay_for_move = max_delay_move; // 600 * 100ms -> 1min
    dst_loc_x = rand() % CHUNK_SIZE;
    dst_loc_y = rand() % CHUNK_SIZE;
}

AnimalServer::AnimalServer(BaseAnimal* base) : Animal(base)
{
    delay_for_move = max_delay_move; // 600 * 100ms -> 1min
    dst_loc_x = rand() % CHUNK_SIZE;
    dst_loc_y = rand() % CHUNK_SIZE;
}

void AnimalServer::move()
{
    delay_for_move--;
    if (delay_for_move)
        return;
    delay_for_move = max_delay_move;

    ItemLocation l = location;
    int _x = location.data.chunk.x;
    int _y = location.data.chunk.y;

    if ((_x == dst_loc_x && _y == dst_loc_y) /*|| (rand() % 5 ==1)*/)
    {
        dst_loc_x = rand() % CHUNK_SIZE;
        dst_loc_y = rand() % CHUNK_SIZE;
    }
    else
    {
        if (rand() % 2)
        {
            if (_x < dst_loc_x)
                _x++;
            else
                _x--;
        }
        if (rand() % 2)
        {
            if (_y < dst_loc_y)
                _y++;
            else
                _y--;
        }
    }
    //  printf("%d, %d -> dst[%d, %d]\n", _x, _y, dst_loc_x, dst_loc_y);

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

    location.data.chunk.x = _x;
    location.data.chunk.y = _y;

    update_location(uid, l, location);
}

bool AnimalServer::tick()
{
    move();
    return Being::tick();
}

PlantServer::PlantServer()
{
    delay_for_grow = max_delay_grow;
}

PlantServer::PlantServer(BasePlant* base) : Plant(base)
{
    delay_for_grow = max_delay_grow;
}

bool PlantServer::grow()
{
    delay_for_grow--;
    if (delay_for_grow)
        return false;
    delay_for_grow = max_delay_grow;

    if (grown)
        return false;
    if (!water)
        return !grown;
    // water--;
    age->value++;

    if (age->value >= max_age->value)
    {
        if (phase != Plant_fruits)
        {
            grown = true;
            change_phase(Plant_fruits);
#ifndef CORE_TUI
            objects_to_update.add(this);
#endif
        }
        return !grown;
    }
    if (age->value >= flowers_time)
    {
        if (phase != Plant_flowers)
        {
            change_phase(Plant_flowers);
#ifndef CORE_TUI
            objects_to_update.add(this);
#endif
        }
        return !grown;
    }
    if (age->value >= growing_time)
    {
        if (phase != Plant_growing)
        {
            change_phase(Plant_growing);
#ifndef CORE_TUI
            objects_to_update.add(this);
#endif
        }
        return !grown;
    }
    if (age->value >= seedling_time)
    {
        if (phase != Plant_seedling)
        {
            change_phase(Plant_seedling);
#ifndef CORE_TUI
            objects_to_update.add(this);
#endif
        }
        return !grown;
    }
    return !grown;
}
/*
bool PlantServer::tick()
{
    grow();
    Plant::tick();
    return true;
}*/
AnimalServer* create_animal(BaseAnimal* base)
{
    return new AnimalServer(base);
}

PlantServer* create_plant(BasePlant* base)
{
    return new PlantServer(base);
}

Element* create_element(BaseElement* base)
{
    return new Element(base);
}
