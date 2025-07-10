#include "craft.h"

#include "networking.h"
#include "tools/axe.h"
#include "tools/knife.h"
#include "world_server.h"
#include "craft_ing.h"
#include "craft_prod.h"

#include <cstdio>

// extern class Player player;
// extern int active_hotbar;
int active_hotbar = 0;

bool craft(int product_id, int ingredients_num, const size_t * ingredients_ids, Player * player)
{
    // printf("id in craft %ld\n", ingredients_ids[0]);
    InventoryElement * crafted = nullptr;
    if (product_id < ING_NUM)
    {
        printf("crafting ingredient %d\n", product_id);
        if (ingredients_num < 1)
            return false;

        InventoryElement * el = player->get_item_by_uid(ingredients_ids[0]);
        if (!el)
        {
            printf("craft from invalid element");
            return false;
        }

        crafted=craft_ing(product_id, el);
    }
    else
    {
        product_id -= ING_NUM;
        if (ingredients_num != 2)
        {
            printf("invalid num of ingredients %d\n", ingredients_num);
            return false;
        }

        printf("crafting product %d", product_id);
        InventoryElement * el1 = player->get_item_by_uid(ingredients_ids[0]);
        InventoryElement * el2 = player->get_item_by_uid(ingredients_ids[1]);
        if (!el1 || !el2)
        {
            printf("craft from invalid element");
            return false;
        }

        crafted = craft_prod(product_id, el1, el2);
    }
    if (crafted)
    {
        world_table[player->map_y][player->map_x]->add_object(crafted, player->x, player->y);
        objects_to_create.add(crafted);
        printf("crafted\n");
        return true;
    }
    else
    {
        printf("failed to craft\n");
    }
    return false;
}


