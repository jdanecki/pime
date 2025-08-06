#include "world.h"
#include "player.h"

chunk * world_table[WORLD_SIZE][WORLD_SIZE];
Chunk_state loaded_chunks[WORLD_SIZE][WORLD_SIZE];

void remove_from_chunks(InventoryElement * object)
{
    world_table[object->location.chunk.map_y][object->location.chunk.map_x]->remove_object(object);
}

//FIXME do we stil need these get_*_at functions?
// Being ** get_being_at(int chunk_x, int chunk_y, int x, int y)
// {
//     // TODO
//     /*for (int i = 0; i < 128; i++)
//     {
//         Being *b = world_table[chunk_y][chunk_x]->beings[i];
//         if (b)
//         {
//             int b_x, b_y;
//             b->get_posittion(&b_x, &b_y);

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
Animal ** get_animal_at_ppos(Player * player)
{
    return get_animal_at(player->map_x, player->map_y, player->x, player->y);
}

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

Object ** get_object_at_ppos(Player * player)
{
    return get_object_at(player->map_x, player->map_y, player->x, player->y);
}


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

Plant ** get_plant_at_ppos(Player * player)
{
    return get_plant_at(player->map_x, player->map_y, player->x, player->y);
}

InventoryElement * get_item_at(int chunk_x, int chunk_y, int x, int y)
{
    chunk * ch = world_table[chunk_y][chunk_x];
    if (!ch)
        return nullptr;
    ListElement * le = ch->objects.head;
    while (le)
    {
        if (le->el->get_x() == x && le->el->get_y() == y)
        {
            return le->el;
        }
        le = le->next;
    }
    return nullptr;
}

InventoryElement * get_item_at_ppos(Player * player)
{
    return get_item_at(player->map_x, player->map_y, player->x, player->y);
}

void set_item_at(InventoryElement * item, int chunk_x, int chunk_y, int x, int y)
{
    // TODO
    /*for (int i = 0; i < 128; i++)
    {
        if (!world_table[chunk_y][chunk_x]->items[i])
        {
            world_table[chunk_y][chunk_x]->items[i] = item;
            break;
        }
    }*/
}

void set_item_at_ppos(InventoryElement * item, Player * player)
{
    set_item_at(item, player->map_x, player->map_y, player->x, player->y);
}
