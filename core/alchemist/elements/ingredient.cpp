#include <cstdio>
#include "ingredient.h"

Ingredient::Ingredient(Ingredient_id i) : InventoryElement(Class_Ingredient), quality("quality", 0), resilience("resilience", 0), usage("usage", 0)
{
    id = i;
}

void Ingredient::show(bool details)
{
    CONSOLE_LOG("(ING)name=%s id=%d\n", get_name(), id);
    InventoryElement::show(details);
    if (!details)
        return;
    quality.show();
    resilience.show();
    usage.show();
    CONSOLE_LOG("form = %s\n", Form_name[req_form]);
    CONSOLE_LOG("^^^ INGREDIENT ^^^\n");
}

Property ** Ingredient::get_properties(int * count)
{
    Property ** props = new Property *[3];
    props[0] = &quality;
    props[1] = &resilience;
    props[2] = &usage;

    *count = 3;
    return props;
}
size_t Ingredient::get_id()
{
    return (size_t)id;
}
const char * Ingredient::get_name()
{
    return ingredient_name[id];
}
char * Ingredient::get_description()
{
    char * buf = new char[128];
    sprintf(buf, "%s: (%s)", get_class_name(), ingredient_name[id]);
    return buf;
}
bool Ingredient::action(Product_action action, Player * pl)
{
    CONSOLE_LOG("ING: %s %s\n", product_action_name[action], get_name());
    return false;
}
