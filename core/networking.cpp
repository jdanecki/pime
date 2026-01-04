#include "networking.h"
#include "alchemist/elements.h"
#include "tiles.h"
#include "world.h"

#ifndef DISABLE_NPC
#include "npc.h"
#endif

#include <cstring>
#include <stdio.h>

//#include "implementations/playerSDL.h"
//#include "implementations/BeingSDL.h"
//#include "implementations/alchemistSDL.h"

void print_status(int l, const char * format, ...);

#define PLAYER_NUM 16
extern Player * players[PLAYER_NUM];

size_t my_id;

InventoryElement * find_by_uid(size_t uid, int chunk_x, int chunk_y)
{
    if (!world_table[chunk_y][chunk_x])
        return nullptr;
    for (InventoryElement * el : world_table[chunk_y][chunk_x]->objects)
    {
        if (el->uid == uid)
            return el;
    }
    return nullptr;
}

void update_chunk(int32_t x, int32_t y, const chunk_table * data)
{
    data = (chunk_table *)((char *)(data));
    if (!world_table[y][x])
    {
        CONSOLE_LOG("SDL: update_chunk new x=%d y=%d\n", x, y);
        // world_table[y][x] = (chunk*)calloc(1, sizeof(chunk));
        // world_table[y][x]->objects = InvList();
        world_table[y][x] = new chunk(x, y);
        memcpy(world_table[y][x]->table, &data[0], CHUNK_SIZE * CHUNK_SIZE * sizeof(int));
        /*CONSOLE_LOG("got %d items\n[", item_num);
        for (int i = 0; i < 1027 + item_num*20; i++)
        {
           CONSOLE_LOG("%d, ", data[i]);
        }
       CONSOLE_LOG("\n");*/
    }
}

void update_object(const ObjectData * data)
{
    //        size_t uid = data.inv_element.data.uid;
    Class_id c_id = data->inv_element.data.c_id;

    InventoryElement * el = find_by_uid(data->inv_element.data.uid, data->inv_element.data.location.chunk.map_x, data->inv_element.data.location.chunk.map_y);
    //FIXME why we get el=NULL? -> change this to get_object_by_uid
    //  CONSOLE_LOG("update_object: el=%p chunk[%d,%d]\n", el, data->inv_element.data.location.chunk.map_x, data->inv_element.data.location.chunk.map_y);
    if (el)
    {
       // CONSOLE_LOG("update_object: el->cid=%x c_id=%x\n", el->c_id, c_id);
    }
    if (el && el->c_id == c_id)
    {
        // CONSOLE_LOG("SDL update_object: %s %s\n", class_name[c_id],  el->get_name());
        switch (c_id)
        {
            case Class_Element:
            {
                Element * element = dynamic_cast<Element *>(el);
                *element = data->element.data;
                break;
            }
            case Class_Ingredient:
            {
                Ingredient * ing = dynamic_cast<Ingredient *>(el);
                *ing = data->ingredient.data;
                break;
            }
            case Class_Product:
            {
                Product * prod = dynamic_cast<Product *>(el);
                *prod = data->product.data;
                break;
            }
            case Class_Plant:
            {
                Plant * plant = dynamic_cast<Plant *>(el);
                *plant = data->plant.data;
                // CONSOLE_LOG("%s size=%f\n", plant->get_name(), plant->size);
                break;
            }
            case Class_Animal:
            {
                Animal * animal = dynamic_cast<Animal *>(el);
                *animal = data->animal.data;
                //     CONSOLE_LOG("%s size=%f\n", animal->get_name(), animal->size);
                break;
            }
            case Class_Player:
            {
                Player * player = dynamic_cast<Player *>(el);
                CONSOLE_LOG("update_object: player=%s inv.elements=%d\n", player->get_name(), player->inventory.nr_elements);
                *player = data->player.data;
                CONSOLE_LOG("update_object: -> update: inv.elements=%d\n", player->inventory.nr_elements);
                break;
            }
            default:
                break;
        }
        // CONSOLE_LOG("%s updated\n", el->get_name());
    }
    else
    {
        /*if (el)
            print_status(1, "bad data for update object %ld %d real %d", uid, c_id, el->c_id);
        else
            print_status(1, "non existing object for update object %ld %d", uid, c_id);*/
    }
}

void create_object(const ObjectData * data)
{
    NetworkObject * object = el_from_data(data);
    if (object)
    {
        register_object(object);
        if (object->c_id != Class_Clan) 
        {
            InventoryElement* el = (InventoryElement*)object;
            add_object_to_world(el, el->location);
        }
    }
    else
    {
        CONSOLE_LOG("SDL: inexisting chunk\n");
    }
}

void failed_craft()
{
    CONSOLE_LOG("craft FAILED\n");
    print_status(1, "failed craft");
}

void action_failed()
{
    CONSOLE_LOG("action FAILED\n");
    print_status(1, "action failed");
}
