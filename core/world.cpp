#include "world.h"
#include "player.h"

chunk * world_table[WORLD_SIZE][WORLD_SIZE];
Chunk_state loaded_chunks[WORLD_SIZE][WORLD_SIZE];

void remove_from_chunks(InventoryElement * object)
{
    world_table[object->location.chunk.map_y][object->location.chunk.map_x]->remove_object(object);
}

void add_object_to_world(InventoryElement* object, ItemLocation location)
{
    object->location = location;
    switch (location.tag)
    {
        case ItemLocation::Tag::Chunk:
            if (world_table[location.chunk.map_y][location.chunk.map_x])
                world_table[location.chunk.map_y][location.chunk.map_x]->add_object(object, location.chunk.x, location.chunk.y);
            break;
        case ItemLocation::Tag::Player:
            abort();
    }
}

void show_chunk(ItemLocation loc)
{
    switch (loc.tag)
    {
        case ItemLocation::Tag::Chunk :
            world_table[loc.chunk.map_y][loc.chunk.map_x]->show();
            break;
    }
}

InventoryElement* find_in_world(ItemLocation* loc, size_t uid)
{
    switch (loc->tag)
    {
        case ItemLocation::Tag::Chunk:
            return world_table[loc->chunk.map_y][loc->chunk.map_x]->find_by_id(uid);
        case ItemLocation::Tag::Player:
            abort();
            return nullptr;
    }
}

int get_world_x(ItemLocation loc)
{
    return loc.chunk.map_x * CHUNK_SIZE + loc.chunk.x;    
}
int get_world_y(ItemLocation loc)
{
    return  loc.chunk.map_y * CHUNK_SIZE + loc.chunk.y;
}

int get_tile_at(ItemLocation loc)
{
    return world_table[loc.chunk.map_y][loc.chunk.map_x]->table[loc.chunk.y][loc.chunk.x].tile;
}



//FIXME do we stil need these get_*_at functions?
//             if (b_x == x && b_y == y)
//             {
//                 return &world_table[chunk_y][chunk_x]->beings[i];
//             }
//         }
//     }*/
//     return NULL;
// }

// Being ** get_being_at_ppos(Player * player)
// {
//     return get_being_at(player->map_x, player->map_y, player->x, player->y);
// }

Animal ** get_animal_at(int chunk_x, int chunk_y, int x, int y)
{
    // TODO
    /*for (int i = 0; i < 128; i++)
    {
        Animal *a = world_table[chunk_y][chunk_x]->animals[i];
        if (a)
        {
            int b_x, b_y;
            a->get_posittion(&b_x, &b_y);

            if (b_x == x && b_y == y)
            {
                return &world_table[chunk_y][chunk_x]->animals[i];
            }
        }
    }*/
    return NULL;
}
// Animal ** get_animal_at_ppos(Player * player)
// {
    // return get_animal_at(player->map_x, player->map_y, player->x, player->y);
// }

Object ** get_object_at(int chunk_x, int chunk_y, int x, int y)
{
    // TODO
    /*for (int i = 0; i < 128; i++)
    {
        Object * o = world_table[chunk_y][chunk_x]->objects[i];
        if (o)
        {
            int b_x, b_y;
            o->get_posittion(&b_x, &b_y);

            if (b_x == x && b_y == y)
            {
                return &world_table[chunk_y][chunk_x]->objects[i];
            }
        }
    }*/
    return NULL;
}

// Object ** get_object_at_ppos(Player * player)
// {
//     return get_object_at(player->map_x, player->map_y, player->x, player->y);
// }

Plant ** get_plant_at(int chunk_x, int chunk_y, int x, int y)
{
    // TODO
    /*for (int i = 0; i < 128; i++)
    {
        Plant *p = world_table[chunk_y][chunk_x]->plants[i];
        if (p)
        {
            int b_x, b_y;
            p->get_posittion(&b_x, &b_y);

            if (b_x == x && b_y == y)
            {
                return &world_table[chunk_y][chunk_x]->plants[i];
            }
        }
    }*/
    return NULL;
}

// Plant ** get_plant_at_ppos(Player * player)
// {
//     return get_plant_at(player->map_x, player->map_y, player->x, player->y);
// }

InventoryElement * get_item_at(ItemLocation loc)
{
    if (loc.tag == ItemLocation::Tag::Player)
        abort();
    
    chunk * ch = world_table[loc.chunk.map_y][loc.chunk.map_x];
    if (!ch)
        return nullptr;
    ListElement * le = ch->objects.head;
    while (le)
    {
        if (le->el->get_x() == loc.chunk.x && le->el->get_y() == loc.chunk.y)
        {
            return le->el;
        }
        le = le->next;
    }
    return nullptr;
}

InventoryElement * get_item_at_ppos(Player * player)
{
    return get_item_at(player->location);
}

// FIXME do we need these functions
