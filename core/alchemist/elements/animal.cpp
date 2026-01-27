#include <cstdio>

#include "animal.h"
#include "../el_list.h"
#include "../random_functions.h"

Animal::Animal(BaseAnimal * b) : InventoryElement(Class_Animal), base(b)
{
    c_id = Class_Animal;

    wild = true;
    dimensions.max_width = random_float_range(0.3, 2.0);
    dimensions.max_height = random_float_range(0.3, 2.0);
}

Animal::Animal(int id) : base(get_base_animal(id))
{
}

void Animal::show(bool details)
{
    InventoryElement::show(details);
    if (details)
    {
        get_base()->show(details);
        CONSOLE_LOG("wild: %s\n", wild ? "yes" : "no");
    }
}
size_t Animal::get_id()
{
    return get_base()->uid;
}
Class_id Animal::get_base_cid()
{
    return get_base()->c_id;
}
bool Animal::action(Product_action action, Player * pl)
{
    CONSOLE_LOG("ANIMAL: %s %s\n", product_action_name[action], get_name());
    return false;
}
const char * Animal::get_name()
{
    return get_base()->get_name();
}
char * Animal::get_description()
{
    char * buf = new char[128];
    sprintf(buf, "%s: (%s)", get_class_name(), get_name());
    return buf;
}
BaseAnimal * Animal::get_base()
{
    return base.get();
}

BaseAnimal * get_base_animal(size_t id)
{
    BaseListElement * el = (BaseListElement *)base_animals.find(&id);
    if (!el)
        return nullptr;
    return (BaseAnimal *)((el)->get_el());
}
Animal::~Animal()
{
}
