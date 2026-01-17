#include "chunk.h"
#include "places/places.h"
#include "generator/generator.h"
#include "generator/random_functions.h"

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
    CONSOLE_LOG("load_chunk(%d, %d)\n", cx, cy);
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

    for (int y = 0; y < CHUNK_SIZE; y++)
        for (int x = 0; x < CHUNK_SIZE; x++)
        {
            InventoryElement * closest_el = nullptr;
            int min_dist = INT_MAX;
            ListElement * cur = ch->objects.head;
            while (cur)
            {
                InventoryElement * el = static_cast<InventoryElement *>(cur->get_el());
                int dx = x - el->location.get_x();
                int dy = y - el->location.get_y();
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
                ch->table[y][x].tile = closest_el->get_id();
            }
        }

    for (int i = 0; i < r->animals_count; i++)
    {
        do_times(r->animals_types[i]->value, add_animal, ch, r->animals_types[i]->animal->id);
    }

    // ch->add_object(create_scroll(new Base(rand() % 10, Class_Scroll,"scroll")));

    size_t id;
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

void load_chunks()
{
    for (int cy = 127; cy < 130; cy++)
        for (int cx = 127; cx < 130; cx++)
            load_chunk(cy, cx);
}
