#include "tiles.h"

void chunk::add_object(InventoryElement * object, int x, int y)
{
#ifndef CORE_FOR_CLIENT
//    printf("adding object %s uid=%lx\n", object->get_name(), object->uid);
#endif
    if (object->c_id == Class_Player)
        objects.add(object);
    else
        objects.add_front(object);

    if (object->c_id == Class_Plant || object->c_id == Class_Animal)
    {
        beings.add(object);
    }

    object->location.tag = ItemLocation::Tag::Chunk;
    object->location.chunk.map_y = map_y;
    object->location.chunk.map_x = map_x;
    object->location.chunk.x = x;
    object->location.chunk.y = y;
}

void chunk::add_object(InventoryElement * object)
{
    int x = rand() % CHUNK_SIZE;
    int y = rand() % CHUNK_SIZE;
    add_object(object, x, y);
}

void chunk::remove_object(InventoryElement * object)
{
    objects.remove(object);
    beings.remove(object);
}

InventoryElement * chunk::find_by_id(size_t id)
{
    for (InventoryElement* el: objects)
    {
        if (el->uid == id)
            return el;
    }
    return nullptr;
}

chunk::chunk(int map_x, int map_y) : map_x(map_x), map_y(map_y)
{
}

void chunk::show()
{
    printf("chunk[%d, %d] obj=%d bei=%d tile=%d\n", map_x, map_y, objects.nr_elements, beings.nr_elements, table[0][0].tile);
    if (objects.nr_elements)
        objects.show(false);
}
