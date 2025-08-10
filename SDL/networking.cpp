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

InventoryElement * remove_from_location(ItemLocation location, size_t id)
{
    InventoryElement * el;
    switch (location.tag)
    {
        case ItemLocation::Tag::Chunk:
        {
            // printf("removed %ld from chunk %d %d\n", id, location.chunk.map_x, location.chunk.map_y);
            if (!world_table[location.chunk.map_y][location.chunk.map_x])
                return nullptr;
            el = world_table[location.chunk.map_y][location.chunk.map_x]->find_by_id(id);
            world_table[location.chunk.map_y][location.chunk.map_x]->remove_object(el);
            break;
        }
        case ItemLocation::Tag::Player:
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

InventoryElement * el_from_data(const ObjectData * data)
{
    InventoryElement * el = nullptr;
    switch (data->tag)
    {
        case ObjectData::Tag::InvElement:
            break;
        case ObjectData::Tag::Element:
            el = new ElementSDL(data->element.data);
            break;
        case ObjectData::Tag::Scroll:
            el = new ScrollSDL(data->scroll.data);
            break;
        case ObjectData::Tag::Ingredient:
            el = new IngredientSDL(data->ingredient.data);
            break;
        case ObjectData::Tag::Product:
            el = new ProductSDL(data->product.data);
            break;
        case ObjectData::Tag::Plant:
        {
            el = new PlantSDL(data->plant.data);
            break;
        }
        case ObjectData::Tag::Animal:
            el = new AnimalSDL(data->animal.data);
            break;
    }
    return el;
}

extern "C"
{

    void knowledge_update(size_t pl_id, Class_id cid, int id)
    {
        printf("knowledge update for %ld cid=%s id=%d\n", pl_id, class_name[cid], id);
    }

    void update_player(uintptr_t id, int32_t map_x, int32_t map_y, int32_t x, int32_t y, int thirst, int hunger)
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
            p->thirst = thirst;
            p->hunger = hunger;

            //  printf("updated player %ld: %d %d %d %d\n", id, map_x, map_y, x, y);
        }
    }

    void update_chunk(int32_t x, int32_t y, const chunk_table * data)
    {
        data = (chunk_table *)((char *)(data) + 3);
        if (!world_table[y][x])
        {
            printf("SDL: update_chunk new x=%d y=%d\n", x, y);
            // world_table[y][x] = (chunk*)calloc(1, sizeof(chunk));
            // world_table[y][x]->objects = InvList();
            world_table[y][x] = new chunk(x, y);
            memcpy(world_table[y][x]->table, &data[0], CHUNK_SIZE * CHUNK_SIZE * sizeof(int));
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
        init_sentences();
        init_questions();
        init_answers();
        printf("got id %ld\n", id);
        print_status(1, "player %d connected", id);
    }

    void update_object(const ObjectData * data)
    {
        //        size_t uid = data.inv_element.data.uid;
        Class_id c_id = data->inv_element.data.c_id;

        InventoryElement * el = find_by_uid(data->inv_element.data.uid, data->inv_element.data.location.chunk.map_x, data->inv_element.data.location.chunk.map_y);

        if (el && el->c_id == c_id)
        {
            //  printf("SDL update_object: %s %s\n", class_name[c_id],  el->get_name());
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
                    //  printf("%s size=%f\n", plant->get_name(), plant->size);
                    break;
                }
                case Class_Animal:
                {
                    Animal * animal = dynamic_cast<Animal *>(el);
                    *animal = data->animal.data;
                    //      printf("%s size=%f\n", animal->get_name(), animal->size);
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
        ItemLocation & old_loc = data.old;
        ItemLocation & new_loc = data.new_;

        InventoryElement * el = remove_from_location(old_loc, id);
        if (!el)
        { // FIXME
            /*printf("SDL: not found item %lu to remove on chunk [%d,%d][%d,%d]->[%d,%d][%d,%d]\n",
                id,
                old_loc.chunk.map_x, old_loc.chunk.map_y,
                old_loc.chunk.x, old_loc.chunk.y,
                new_loc.chunk.map_x, new_loc.chunk.map_y,
                new_loc.chunk.x, new_loc.chunk.y);
              */
            return;
        }
        switch (new_loc.tag)
        {
            case ItemLocation::Tag::Chunk:
            {
                /*     printf("SDL: update item location %s:%s on chunk [%d,%d][%d,%d]->[%d,%d][%d,%d]\n",
                         el->get_class_name(), el->get_name(),
                         old_loc.chunk.map_x, old_loc.chunk.map_y,
                         old_loc.chunk.x, old_loc.chunk.y,
                         new_loc.chunk.map_x, new_loc.chunk.map_y,
                         new_loc.chunk.x, new_loc.chunk.y);
                  */
                ItemLocation old_l;
                ItemLocation new_l;
                old_l.chunk.x = old_loc.chunk.x;
                old_l.chunk.y = old_loc.chunk.y;
                new_l.chunk.x = new_loc.chunk.x;
                new_l.chunk.y = new_loc.chunk.y;
                el->update_item_location(old_l, new_l);
                world_table[new_loc.chunk.map_y][new_loc.chunk.map_x]->add_object(el, new_loc.chunk.x, new_loc.chunk.y);
                break;
            }
            case ItemLocation::Tag::Player:
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

    void create_object(const ObjectData * data)
    {
        InventoryElement * el = el_from_data(data);
        if (el)
        {
            switch (el->location.tag)
            {
                case ItemLocation::Tag::Chunk:
                {
                    int item_x = el->location.chunk.x;
                    int item_y = el->location.chunk.y;
                    int x = el->location.chunk.map_x;
                    int y = el->location.chunk.map_y;
                    if (world_table[y][x])
                    {
                        world_table[y][x]->add_object(el, item_x, item_y);
                    }
                    break;
                }
                case ItemLocation::Tag::Player:
                {
                    int p_id = el->location.player.id;
                    players[p_id]->inventory->add(el);
                    update_hotbar();
                }
            }
            // printf("created object: %s\n", el->get_name());
            // print_status(1, "created object: %s", el->get_name());
        }
        else
        {
            printf("SDL: inexisting chunk\n");
        }
    }

    void destroy_object(uintptr_t id, ItemLocation location)
    {
        InventoryElement * el = remove_from_location(location, id);
        if (el)
        {
            printf("SDL: destroy_object %ld", id);
            delete el;
        }
        else
            printf("SDL: deleting inexisting item %ld\n", id);
    }

    void failed_craft()
    {
        print_status(1, "failed craft");
    }

    void action_failed()
    {
        print_status(1, "action failed");
    }
}

