#include "networking.h"
#include "../core/alchemist/elements.h"
#include "../core/player.h"
#include "../core/tiles.h"
#include "../core/world.h"
#include "implementations/BeingSDL.h"
#include "implementations/alchemistSDL.h"
#include "text.h"
#include <cstring>
#include <stdio.h>

#define PLAYER_NUM 16
extern Player * players[PLAYER_NUM];
extern Player * player;

InventoryElement * find_by_uid(size_t uid, int chunk_x, int chunk_y)
{
    if (!world_table[chunk_y][chunk_x])
       return nullptr;
    ListElement * el = world_table[chunk_y][chunk_x]->objects.head;
    while (el)
    {
        if (el->el->uid == uid)
            return el->el;
        el = el->next;
    }
    return nullptr;
}

void update_hotbar()
{
    // FIXME - remove/add only one element
    for (int i = 0; i < 10; i++)
        player->hotbar[i] = nullptr;
    ListElement * le = player->inventory->head;
    int i = 0;
    while (le)
    {
        if (i >= 10)
            break;
        if (le->el)
            player->hotbar[i] = le->el;
        le = le->next;
        i++;
    }
}

InventoryElement * remove_from_location(ItemLocationLol location, size_t id)
{
    InventoryElement * el;
    switch (location.tag)
    {
        case ItemLocationLol::Tag::Chunk:
        {
            // printf("removed %ld from chunk %d %d\n", id, location.chunk.map_x, location.chunk.map_y);
            if (!world_table[location.chunk.map_y][location.chunk.map_x])
                return nullptr;
            el = world_table[location.chunk.map_y][location.chunk.map_x]->find_by_id(id);
            world_table[location.chunk.map_y][location.chunk.map_x]->remove_object(el);
            break;
        }
        case ItemLocationLol::Tag::Player:
        {
            // printf("removed %ld from player %ld\n", id, location.player.id);
            el = players[location.player.id]->get_item_by_uid(id);
            players[location.player.id]->drop(el);
            if ((int)location.player.id == player->get_id())
            {
                update_hotbar();
            }
        }
    }
    return el;
}

InventoryElement * el_from_data(ObjectData data)
{
    InventoryElement * el = nullptr;
    switch (data.tag)
    {
        case ObjectData::Tag::InvElement:
            break;
        case ObjectData::Tag::Element:
            el = new ElementSDL(&data.element.data);

            break;
        case ObjectData::Tag::Ingredient:
            el = new IngredientSDL(&data.ingredient.data);
            break;
        case ObjectData::Tag::Product:
            el = new ProductSDL(&data.product.data);
            break;
        case ObjectData::Tag::Plant:
        {
            el = new PlantSDL(&data.plant.data);
            break;
        }
        case ObjectData::Tag::Animal:
            el = new AnimalSDL(&data.animal.data);
            break;
    }
    if (el)
    {
        el->uid = data.inv_element.data.uid;
        el->location.type = LOCATION_CHUNK; //(ItemLocationType)data.inv_element.data.location.tag;
        el->location.data.chunk.map_x = data.inv_element.data.location.chunk.map_x;
        el->location.data.chunk.map_y = data.inv_element.data.location.chunk.map_y;
        el->location.data.chunk.x = data.inv_element.data.location.chunk.x;
        el->location.data.chunk.y = data.inv_element.data.location.chunk.y;
    }
    return el;
}

extern "C"
{

    void update_player(uintptr_t id, int32_t map_x, int32_t map_y, int32_t x, int32_t y)
    {
        if (id >= 0 && id < PLAYER_NUM)
        {
            if (!players[id])
            {
                players[id] = new Player(id);
            }
            /*for (int i = 0; i < 25; i++)
            {
                printf("%d, ", data[i]);
            }*/
            Player * p = players[id];
            p->map_x = map_x;
            p->map_y = map_y;

            // FIXME when more chunks added
            if (x != p->x)
                p->going_right = p->x > x ? 0 : 1;
            p->x = x;
            p->y = y;

            // printf("updated player %ld: %d %d %d %d\n", id, player->map_x, player->map_y, player->x, player->y);
        }
    }

    void update_chunk(int32_t x, int32_t y, const chunk_table * data)
    {
        if (!world_table[y][x])
        {
            // world_table[y][x] = (chunk*)calloc(1, sizeof(chunk));
            // world_table[y][x]->objects = InvList();
            world_table[y][x] = new chunk(x, y);
            memcpy(world_table[y][x]->table, &data[0], CHUNK_SIZE * CHUNK_SIZE * sizeof(game_tiles));
            /*printf("got %d items\n[", item_num);
            for (int i = 0; i < 1027 + item_num*20; i++)
            {
                printf("%d, ", data[i]);
            }
            printf("\n");*/
        }
    }

    void got_id(uintptr_t id, int64_t seed)
    {
        // my_id = id;
        players[id] = new Player(id);
        player = players[id];

        printf("seed: %ld\n", seed);
        srand(seed);
        init_elements();
        init_sentences();
        printf("got id %ld\n", id);
        print_status(1, "player %d connected", id);
    }

    void update_object(ObjectData data)
    {
//        size_t uid = data.inv_element.data.uid;
        Class_id c_id = data.inv_element.data.el_type;

        InventoryElement * el = find_by_uid(data.inv_element.data.uid, data.inv_element.data.location.chunk.map_x, data.inv_element.data.location.chunk.map_y);

        if (el && el->c_id == c_id)
        {
            switch (c_id)
            {
                case Class_Element:
                    break;
                case Class_Ingredient:
                    break;
                case Class_Product:
                    break;
                case Class_Plant:
                {
                    Plant * p = dynamic_cast<Plant *>(el);
                    p->phase = data.plant.data.phase;
                    p->grown = data.plant.data.grown;
                    p->age->value = data.plant.data.age;
                    p->max_age->value = data.plant.data.max_age;
                    break;
                }
                case Class_Animal:
                {
                    Animal * p = dynamic_cast<Animal *>(el);
                    p->age->value = data.animal.data.age;
                    p->max_age->value = data.animal.data.max_age;
                    break;
                }
                default:
                    break;
            }
            // printf("%s updated\n", el->get_name());
        }
        else
        {
            /*if (el)
                print_status(1, "bad data for update object %ld %d real %d", uid, c_id, el->c_id);
            else
                print_status(1, "non existing object for update object %ld %d", uid, c_id);*/
        }
    }

    void update_item_location(LocationUpdateData data)
    {
        size_t id = data.id;
        ItemLocationLol & old_loc = data.old;
        ItemLocationLol & new_loc = data.new_;

        InventoryElement * el = remove_from_location(old_loc, id);
        if (!el)
        {
            //print_status(1, "not found item to remove %d %d", old_loc.chunk.map_x, old_loc.chunk.map_y);
            return;
        }
        switch (new_loc.tag)
        {
            case ItemLocationLol::Tag::Chunk:
            {
                // printf("added %ld to chunk %d %d\n", id, new_loc.chunk.map_x, new_loc.chunk.map_y);
                ItemLocation old_l;
                ItemLocation new_l;
                old_l.data.chunk.x = old_loc.chunk.x;
                old_l.data.chunk.y = old_loc.chunk.y;
                new_l.data.chunk.x = new_loc.chunk.x;
                new_l.data.chunk.y = new_loc.chunk.y;
                el->update_item_location(old_l, new_l);
                world_table[new_loc.chunk.map_y][new_loc.chunk.map_x]->add_object(el, new_loc.chunk.x, new_loc.chunk.y);
                break;
            }
            case ItemLocationLol::Tag::Player:
            {
                // printf("added %ld to player %ld\n", id, new_loc.player.id);
                players[new_loc.player.id]->pickup(el);
                if ((int)new_loc.player.id == player->get_id())
                {
                    update_hotbar(); // FIXME - remove only one element
                }
            }
        }
    }

    void create_object(ObjectData data)
    {
        int x = data.inv_element.data.location.chunk.map_x;
        int y = data.inv_element.data.location.chunk.map_y;
        if (world_table[y][x])
        {
            InventoryElement * el = el_from_data(data);
            if (el)
            {
                int item_x = el->location.data.chunk.x;
                int item_y = el->location.data.chunk.y;
                world_table[y][x]->add_object(el, item_x, item_y);

                print_status(1, "created object: %s", el->get_name());
            }
        }
        else
        {
            print_status(1, "inexisting chunk");
        }
    }

    void destroy_object(uintptr_t id, ItemLocationLol location)
    {
        InventoryElement * el = remove_from_location(location, id);
        if (el)
        {
            print_status(1, "delete %ld", id);
            delete el;
        }
        else
            print_status(1, "deleting inexisting item %ld", id);
    }

    void failed_craft()
    {
        print_status(1, "failed craft");
    }
}
