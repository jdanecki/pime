#include "ingredient_server.h"
#include "../elements_server.h"

IngredientServer::IngredientServer(InventoryElement * from, Ingredient_id id, Form f) : Ingredient(id)
{
    req_form = f;
    switch (from->get_cid())
    {
        case Class_Ingredient:
        {
            IngredientServer * ing = static_cast<IngredientServer *>(from);
            el = ing->el;
            break;
        }
        default:
            el = from->get_id();
            break;
    }
}

bool IngredientServer::action(Product_action action, Player * pl)
{
    CONSOLE_LOG("ING_SERVER: %s %s\n", product_action_name[action], get_name());
    return false;
}

void IngredientServer::show(bool details)
{
    CONSOLE_LOG("vvv INGREDIENT_SERVER vvv\n");
    Ingredient::show(details);
    CONSOLE_LOG("el: base_id=%d\n", el);
    CONSOLE_LOG("!!! INGREDIENT_SERVER !!!\n");
}

bool IngredientServer::can_pickup()
{
    return true;
}
