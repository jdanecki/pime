#include "world_server.h"
#include "elements_server.h"
#include <stdarg.h>

void generator()
{
    load_chunk(WORLD_CENTER, WORLD_CENTER);
    printf("generated: %d items\n", world_table[128][128]->objects.nr_elements);
}

void update()
{
    // TODO maybe in the future make it smarter
    for (int y = 0; y < WORLD_SIZE; y++)
    {
        for (int x = 0; x < WORLD_SIZE; x++)
        {
            chunk * c = world_table[y][x];
            if (!c)
                continue;
            ListElement * el = world_table[y][x]->beings.head;
            while (el)
            {
                el->el->tick();
                el = el->next;
            }
        }
    }
    // TODO kill animals
    /*            if (!a->alive)
        {
            int x,y;
            a->get_posittion(&x, &y);

            Element * el;
            switch (a->type)
            {
                case ANIMALID_pig:
                    el = new Element(base_elements[ID_RAW_HAM]);
            }
            el->set_posittion(x, y);
            set_item_at(el, player.map_x, player.map_y, x, y);

            free(a);
            a=NULL;
            world_table[player.map_y][player.map_x]->animals[i]=NULL;
        }*/
}

void print_status(int l, const char * format, ...)
{
    va_list args;
    va_start(args, format);
    vprintf(format, args);
    va_end(args);
    printf("\n");
    fflush(stdout);
}
