#include "knife.h"
#include "../world_server.h"
#include <cstdio>
#include <cstdlib>

Knife::Knife(InventoryElement * el1, InventoryElement * el2) : Product(el1, el2, PROD_KNIFE, Form_solid)
{
}
#if 0
bool Knife::use(int map_x, int map_y, int x, int y)
{
    /*for (int i = 0; i < CHUNK_SIZE*CHUNK_SIZE; i++) {
        Plant * p = NULL; //world_table[map_y][map_x]->plants[i];
        if (p)
        {
            int px,py;
            p->get_posittion(&px, &py);
            if (x == px && y == py)
            {
                switch (p->type) {
                    case PLANTID_strawberry:
                    {
                        if (p->grown)
                        {
                            Element * el = new Element(base_elements[ID_STRAWBERRY]);
                            el->set_posittion(x, y);
                            set_item_at(el, map_x, map_y, x, y);

                            Element * seed_el = new Element(base_elements[ID_STRAWBERRY_SEEDS]);
                            seed_el->set_posittion(x, y);
                            set_item_at(seed_el, map_x, map_y, x, y);

                            free(p);
                            p=NULL;
                            // TODOworld_table[map_y][map_x]->plants[i]=NULL;
                            return true;
                        }
                    }
                }
            }
        }
        Animal * a = NULL; //TODOworld_table[map_y][map_x]->animals[i];
        if (a)
        {
            int ax,ay;
            a->get_posittion(&ax, &ay);
            if (x == ax && y == ay)
                a->alive=false;
        }
    }*/
    return false;
}
#endif

bool Knife::check_ing()
{
    int id1 = ings[0]->get_id();
    int id2 = ings[1]->get_id();

    if ((id1 == ING_KNIFE_BLADE && id2 == ING_KNIFE_HANDLE) || (id1 == ING_KNIFE_HANDLE && id2 == ING_KNIFE_BLADE))
        return true;
    printf("wrong ingredients\n");
    return false;
}
