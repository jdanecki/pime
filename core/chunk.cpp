#include "chunk.h"
#include "alchemist/object.h"
#include "alchemist/random_functions.h"

void chunk::add_object(InventoryElement * object, float x, float y, bool front)
{
    if (front)
        objects.add_front(object);
    else
        objects.add(object);

    Class_id cid = object->c_id;
    if (cid == Class_Animal || cid == Class_Plant || cid == Class_Player || cid == Class_Npc)
    {
        beings.add(object);
    }

    object->location.set_chunk(map_x, map_y, x, y);
}

void chunk::add_object(InventoryElement * object)
{
    int x = random_range(0, CHUNK_SIZE);
    int y = random_range(0, CHUNK_SIZE);
    if (object->get_cid() == Class_Element && object->location.chunk.map_x==128) CONSOLE_LOG("class %d, %d %d, %x\n", object->get_cid(), x, y, object->get_uid());
    add_object(object, x, y);
}

void chunk::remove_object(InventoryElement * object)
{
    objects.remove(object);
    beings.remove(object);
}

InventoryElement * chunk::find_by_id(size_t id)
{
    for (InventoryElement * el : objects)
    {
        if (el->uid == id)
            return el;
    }
    return nullptr;
}

chunk::chunk(int map_x, int map_y) : map_x(map_x), map_y(map_y)
{
    objects.name = "objects";
    beings.name = "beings";
}

void chunk::show()
{
    CONSOLE_LOG("chunk[%d, %d] obj=%d bei=%d tile=%d\n", map_x, map_y, objects.nr_elements, beings.nr_elements, table[0][0].tile);
    if (objects.nr_elements)
        objects.show(false);
}
