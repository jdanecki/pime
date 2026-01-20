#include "base_plant.h"
#include "../names.h"
#include <cstdio>
#include <cstdlib>

BasePlant::BasePlant(int index) : Base(index, Class_BasePlant, create_name(5))
{
    flowers = rand() % 2;
    leaves = rand() % 2;
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
