#include "pickaxe.h"
#include "../networking.h"
#include "../world_server.h"
#include <cstddef>
#include <cstdio>

PickAxe::PickAxe(InventoryElement * el1, InventoryElement * el2) : Product(el1, el2, PROD_PICKAXE, Form_solid)
{
}

/*bool PickAxe::use(int map_x, int map_y, int x, int y)
{
    for (int i = 0; i < CHUNK_SIZE*CHUNK_SIZE; i++) {
        // TODO
        Plant * p = NULL;
        //Plant * p = world_table[map_y][map_x]->plants[i];
        if (p)
        {
            int px,py;
            p->get_posittion(&px, &py);
            if (x == px && y == py && p->grown)
            {
                // TODO    break when used too much
            }
        }

        Object * o = NULL; //TODO world_table[map_y][map_x]->objects[i];
        if (o && o->type == OBJECT_wall)
        {
            int ox,oy;
            o->get_posittion(&ox, &oy);
            if (ox == x && oy == y)
            {
                Element * el = new Element(o->base);
                el->set_posittion(x, y);
                set_item_at(el, map_x, map_y, x, y);

                free(o);
                o=NULL;
                //TODO world_table[map_y][map_x]->objects[i]=NULL;
                return true;
            }
        }
    }
    return false;
}*/

#if 0
bool PickAxe::use(InventoryElement * object)
{


    Plant * p = dynamic_cast<Plant *>(object);

    if (p && (p->type == PLANTID_tree || p->type == PLANTID_tree1 || p->type == PLANTID_tree2))
    {
        Element * el = nullptr;
        Element * el_seed = nullptr;

        // FIXME change this to object->use

switch (p->type)
        {
            case PLANTID_tree:
            {
                el = new Element(base_elements[ID_LOG]);
                el_seed = new Element(base_elements[ID_ACORN]);
                break;
            }
            case PLANTID_tree1:
            {
                el = new Element(base_elements[ID_LOG1]);
                el_seed = new Element(base_elements[ID_ACORN1]);
                break;
            }
            case PLANTID_tree2:
            {
                el = new Element(base_elements[ID_LOG2]);
                el_seed = new Element(base_elements[ID_ACORN2]);
                break;
            }
            default: // FIXME
                return false;
        }

        ItemLocation loc = p->location;
        world_table[loc.data.chunk.map_y][loc.data.chunk.map_x]->add_object(el, loc.data.chunk.x, loc.data.chunk.y);
        world_table[loc.data.chunk.map_y][loc.data.chunk.map_x]->add_object(el_seed, loc.data.chunk.x, loc.data.chunk.y);
        objects_to_create.add(el);
        objects_to_create.add(el_seed);

        destroy(p);
        p = NULL;
        //                    TODO
        // world_table[map_y][map_x]->plants[i]=NULL;
        return true;
    }

    return true;
}
#endif
bool PickAxe::check_ing()
{
    int id1 = ings[0]->get_id();
    int id2 = ings[1]->get_id();

    if ((id1 == ING_PICKAXE_BLADE && id2 == ING_PICKAXE_HANDLE) || (id1 == ING_PICKAXE_HANDLE && id2 == ING_PICKAXE_BLADE))
        return true;
    printf("wrong ingredients\n");
    return false;
}
