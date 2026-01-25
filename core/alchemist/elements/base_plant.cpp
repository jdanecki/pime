#include <cstdio>
#include <cstdlib>

#include "base_plant.h"
#include "../names.h"
#include "alchemist/random_functions.h"

BasePlant::BasePlant(int index) : Base(index, Class_BasePlant, create_name(5))
{
    flowers = random_range(0, 1);
    leaves = random_range(0, 1);
    edible.set_random();
}

void BasePlant::show(bool details)
{
    CONSOLE_LOG("BasePlant:\n");
    CONSOLE_LOG("flowers=%d\n", flowers);
    CONSOLE_LOG("leaves=%d\n", leaves);
    if (details)
        Base::show(details);
}
size_t BasePlant::get_size()
{
    return sizeof(BasePlant);
}
void BasePlant::copy_data(unsigned char * ptr, int i)
{
    BasePlant * dst = &((BasePlant *)ptr)[i];
    *dst = *this;
}
