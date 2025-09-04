#include "tiles.h"

void chunk::add_object(InventoryElement * object, int x, int y)
{
#ifndef CORE_FOR_CLIENT
//    printf("adding object %s uid=%lx\n", object->get_name(), object->uid);
#endif
    objects.add(object);
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
    ListElement * el = objects.head;
    while (el)
    {
        if (el->el->uid == id)
            return el->el;
        el = el->next;
    }
    return nullptr;
}

chunk::chunk(int map_x, int map_y) : map_x(map_x), map_y(map_y)
{
}

void chunk::show()
{
    printf("chunk[%d, %d] obj=%d bei=%d\n", map_x, map_y, objects.nr_elements, beings.nr_elements);
    if (objects.nr_elements)
        objects.show(true);
}
