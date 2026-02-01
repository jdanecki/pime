#include "product_server.h"
#include "ingredient_server.h"

void ProductServer::init(int c, Form f)
{
    ing_count = c;
    req_form = f;
}

ProductServer::ProductServer(InventoryElement * el1, InventoryElement * el2, Product_id id, Form f, int act_cnt) : Product(id, act_cnt)
{
    c_id = Class_Product;
    ings[0] = (static_cast<IngredientServer *>(el1))->el;
    ings[1] = (static_cast<IngredientServer *>(el2))->el;
    init(2, f);
}

/*ProductServer::ProductServer(InventoryElement ** from, int count, Product_id id, Form f, int act_cnt) : Product(id, act_cnt)
{
    c_id = Class_Product;
    ings = (IngredientServer **)from;
    init(count, f);
}
*/
void ProductServer::show(bool details)
{
    CONSOLE_LOG("vvv PRODUCT_SERVER vvv\n");
    Product::show(details);
    for (int i = 0; i < ing_count; i++)
        CONSOLE_LOG("ings[%d] base=%d\n", i, ings[i]);
    CONSOLE_LOG("!!! PRODUCT_SERVER !!!\n");
}

bool ProductServer::use_on(InventoryElement * object, Player * pl)
{
    if (!actions_count)
        return false;
    // FIXME use more actions
    CONSOLE_LOG("%s: %s %s\n", get_name(), product_action_name[actions[0]], object->get_name());
    return object->action(actions[0], pl);
    // FIXME change properties of product after action
}
bool ProductServer::use_tile(int map_x, int map_y, int x, int y, Player * pl)
{
    if (!actions_count)
        return false;
    CONSOLE_LOG("ProductServer(%s): %s tile (%d, %d): (%d, %d)\n", get_name(), product_action_name[actions[0]], map_x, map_y, x, y);
    return true;
}
bool ProductServer::can_pickup()
{
    return true;
}
