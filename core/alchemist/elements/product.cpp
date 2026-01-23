#include <cstdio>
#include <cassert>

#include "product.h"

Product::Product(Product_id id, int actions_count) : InventoryElement(Class_Product), quality("quality", 0), resilience("resilience", 0), usage("usage", 0), actions_count(actions_count), id(id)
{
    c_id = Class_Product;

    assert(actions_count < 10);

    for (int i = 0; i < 10; i++)
    {
        actions[i] = ACT_NOTHING;
    }
}

void Product::show(bool details)
{
    CONSOLE_LOG("(PROD)name=%s id=%d\n", get_name(), id);
    InventoryElement::show(details);
    if (!details)
        return;
    quality.show();
    resilience.show();
    usage.show();
    for (int i = 0; i < actions_count; i++)
    {
        CONSOLE_LOG("action[%d]: %d -> %s\n", i, actions[i], product_action_name[actions[i]]);
    }
    CONSOLE_LOG("^^^ PRODUCT ^^^\n");
}

size_t Product::get_id()
{
    return (size_t)id;
}
void Product::add_action(Product_action * a)
{
}
Property ** Product::get_properties(int * count)
{
    Property ** props = new Property *[3];
    props[0] = &quality;
    props[1] = &resilience;
    props[2] = &usage;

    *count = 3;
    return props;
}
bool Product::check_ing()
{
    return false;
}
const char * Product::get_name()
{
    return product_name[id];
}
char * Product::get_description()
{
    char * buf = new char[128];
    sprintf(buf, "%s: (%s)", get_class_name(), product_name[id]);
    return buf;
}
