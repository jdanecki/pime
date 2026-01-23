#include <cstdio>
#include <cstdlib>

#include "base_animal.h"
#include "../names.h"

BaseAnimal::BaseAnimal(int index) : Base(index, Class_BaseAnimal, create_name(7))
{
    carnivorous = rand() % 2;
    swimming = rand() % 2;
    flying = rand() % 2;
    edible.set_random();
}

void BaseAnimal::show(bool details)
{
    CONSOLE_LOG("BaseAnimal:\n");
    CONSOLE_LOG("carnivorous=%d\n", carnivorous);
    CONSOLE_LOG("swimming=%d\n", swimming);
    CONSOLE_LOG("flying=%d\n", flying);
    if (details)
        Base::show(details);
}
size_t BaseAnimal::get_size()
{
    return sizeof(BaseAnimal);
}
void BaseAnimal::copy_data(unsigned char * ptr, int i)
{
    BaseAnimal * dst = &((BaseAnimal *)ptr)[i];
    *dst = *this;
}
