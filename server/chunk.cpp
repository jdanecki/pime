#include "chunk.h"
#include "places/places.h"
#include "generator/generator.h"
#include "../core/alchemist/random_functions.h"

typedef void (*callback_fn)(chunk * ch, size_t id);

void do_times(float prob, callback_fn f, chunk * ch, int id)
{
    int count = (int)(prob * 2.0f);
    for (int i = 0; i < count; ++i)
    {
        if (random_bool(0.5))
        {
            f(ch, id);
        }
    }
}

void add_place(chunk * ch, Place_id id, int x, int y)
{
    ch->add_object(create_place(id), x, y);
}

void add_element(chunk * ch, size_t id)
{
    BaseListElement * base_el = (BaseListElement *)base_elements.find(&id);
    ch->add_object(create_element((BaseElement *)(base_el->get_el())));
}

void add_plant(chunk * ch, size_t id)
{
    BaseListElement * base_el = (BaseListElement *)base_plants.find(&id);
    ch->add_object(create_plant((BasePlant *)(base_el->get_el())));
}

void add_animal(chunk * ch, size_t id)
{
    BaseListElement * base_el = (BaseListElement *)base_animals.find(&id);
    ch->add_object(create_animal((BaseAnimal *)(base_el->get_el())));
}

void load_chunk(int cx, int cy)
{
    //  CONSOLE_LOG("load_chunk(%d, %d)\n", cx, cy);
    chunk * ch = new chunk(cx, cy);
    Region * r = find_region(cx, cy);

try_again:
    for (int i = 0; i < r->rocks_count; i++)
    {
        do_times(r->rocks_types[i]->value, add_element, ch, r->rocks_types[i]->terrain->id);
    }
    for (int i = 0; i < r->plants_count; i++)
    {
        do_times(r->plants_types[i]->value, add_plant, ch, r->plants_types[i]->plant->id);
    }

    if (!ch->objects.nr_elements)
    {
        CONSOLE_LOG("empty chunk, trying again %d\n", r->rocks_count);
        goto try_again;
    }
    /*size_t id1=0;
    BaseListElement * base_el1 = (BaseListElement *)base_elements.find(&id1);
    ch->add_object(create_element((BaseElement *)(base_el1->get_el())), 10, 9);

    size_t id2=1;
    BaseListElement * base_el2 = (BaseListElement *)base_elements.find(&id2);
    ch->add_object(create_element((BaseElement *)(base_el2->get_el())), 5, 4);

*/
    for (int y = 0; y < CHUNK_SIZE; y++)
        for (int x = 0; x < CHUNK_SIZE; x++)
        {
            InventoryElement * closest_el = nullptr;
            int min_dist = INT_MAX;
            ListElement * cur = ch->objects.head;
            while (cur)
            {
                InventoryElement * el = static_cast<InventoryElement *>(cur->get_el());
                int dx = x - el->location.get_tile_x();
                int dy = y - el->location.get_tile_y();
                int dist = dx * dx + dy * dy;
                if (dist < min_dist)
                {
                    min_dist = dist;
                    closest_el = el;
                }
                cur = cur->next;
            }
            if (closest_el)
            {
                ch->table[y][x].tile = closest_el->get_id() % TILE_TEXTURES;
            }

             // ch->table[y][x].tile = (x + y) % TILE_TEXTURES;
        }

    for (int i = 0; i < r->animals_count; i++)
    {
        do_times(r->animals_types[i]->value, add_animal, ch, r->animals_types[i]->animal->id);
    }

    // ch->add_object(create_scroll(new Base(rand() % 10, Class_Scroll,"scroll")));

    world_table[cy][cx] = ch;
}

void show_chunk()
{
    ListElement * pl_el = players->head;
    if (!pl_el)
        return;
    PlayerClient * pl = (PlayerClient *)pl_el;
    int x = pl->player->location.chunk.map_x;
    int y = pl->player->location.chunk.map_y;
    CONSOLE_LOG("Player@[%d,%d]\n", x, y);
    chunk * ch = world_table[y][x];
    ch->show();
    Region * reg = find_region(x, y);
    reg->show();
}

void show_loaded_chunks();

void load_chunks()
{
    int size = 10;
    CONSOLE_LOG("Loading chunks: %d - %d \n", 128 - size, 128 + size);

    for (int cy = 128 - size; cy <= 128 + size; cy++)
        for (int cx = 128 - size; cx <= 128 + size; cx++)
            load_chunk(cy, cx);
    CONSOLE_LOG("Chunks loaded\n");
    show_loaded_chunks();
}
