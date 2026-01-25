#include <cstdio>
#include <cstdlib>

#include "plant.h"
#include "../el_list.h"
#include "alchemist/random_functions.h"

void Plant::init(BasePlant * b)
{
    base = b;
    seedling_time = 10 + random_range(10, 30);
    growing_time = random_range(seedling_time, seedling_time + 150);
    flowers_time = random_range(growing_time, growing_time + 30);
    phase = (Plant_phase)(rand() % (Plant_fruits + 1));
    grown = false;
    water = random_range(0, 99);
}

Plant::Plant(BasePlant * b) : InventoryElement(Class_Plant), base(b)
{
    init(b);
}

Plant::Plant(int id) : base(get_base_plant(id))
{
}

void Plant::show(bool details)
{
    InventoryElement::show(details);
    CONSOLE_LOG("%s\n", get_base()->get_name());
    if (details)
    {
        get_base()->show(details);
        CONSOLE_LOG("phase=%s grown=%d times=%d/%d/%d/ water=%d \n", plant_phase_name[phase], grown, seedling_time, growing_time, flowers_time, water);
    }
}
BasePlant * Plant::get_base()
{
    return base.get();
}
const char * Plant::get_name()
{
    return get_base()->get_name();
}
size_t Plant::get_id()
{
    return get_base()->uid;
}
Class_id Plant::get_base_cid()
{
    return get_base()->c_id;
}
bool Plant::action(Product_action action, Player * pl)
{
    CONSOLE_LOG("PLANT: %s %s\n", product_action_name[action], get_name());
    return false;
}
char * Plant::get_description()
{
    char * buf = new char[128];
    sprintf(buf, "%s: (%s) %s", get_class_name(), get_name(), plant_phase_name[phase]);
    return buf;
}
Property ** Plant::get_properties(int * count)
{
    Property ** props = new Property *[1];
    props[0] = new Property(plant_phase_name[phase], phase);

    *count = 1;
    return props;
}

BasePlant * get_base_plant(size_t id)
{
    BaseListElement * el = (BaseListElement *)base_plants.find(&id);
    if (!el)
        return nullptr;
    return (BasePlant *)((el)->get_el());
}
