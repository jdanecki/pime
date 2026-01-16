#include "craft.h"

#include "networking.h"
#include "world_server.h"
#include "craft_ing.h"
#include "craft_prod.h"
#include <stdio.h>

// extern class Player player;
// extern int active_hotbar;
int active_hotbar = 0;

bool craft_entry(int product_id, int ingredients_num, const size_t * ingredients_ids, Player * player)
{
    // CONSOLE_LOG("id in craft %ld\n", ingredients_ids[0]);
    InventoryElement * crafted = nullptr;
    if (product_id < ING_COUNT)
    {
        CONSOLE_LOG("crafting ingredient %d -> %s\n", product_id, ingredient_name[product_id]);
        if (ingredients_num < 1)
        {
            CONSOLE_LOG("too many to craft ingredient\n");
            return false;
        }
        InventoryElement * el = player->get_item_by_uid(ingredients_ids[0]);
        if (!el)
        {
            CONSOLE_LOG("craft from invalid element\n");
            return false;
        }

        crafted = craft_ing(product_id, el);
    }
    else
    {
        product_id -= ING_COUNT;
        if (ingredients_num != 2)
        {
            CONSOLE_LOG("invalid num of ingredients %d\n", ingredients_num);
            return false;
        }

        CONSOLE_LOG("crafting product %d -> %s\n", product_id, product_name[product_id]);
        InventoryElement * el1 = player->get_item_by_uid(ingredients_ids[0]);
        InventoryElement * el2 = player->get_item_by_uid(ingredients_ids[1]);
        if (!el1 || !el2)
        {
            CONSOLE_LOG("craft from invalid element\n");
            return false;
        }

        crafted = craft_prod(product_id, el1, el2);
    }
    if (crafted)
        add_object_to_world(crafted, player->location);
    else
        CONSOLE_LOG("Can't craft\n");

    return crafted != nullptr;
}
