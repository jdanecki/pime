#include <cstdio>
#include "base.h"
#include "alchemist/elements/types.h"

Base::Base(int index, Class_id c, const char * name) : name(name), NetworkObject(c, index)
{
    edible.eating_by = 1; // only by animals
}

void Base::show(bool details)
{
    CONSOLE_LOG("Base name=%s class:%s id=%ld \n", get_name(), class_name[c_id], uid);
    if (details)
        edible.show();
}

const char * Base::get_name()
{
    return name.str;
}

Base::~Base()
{
}
size_t Base::get_size()
{
    return sizeof(Base);
}
void Base::copy_data(unsigned char * ptr, int i)
{
}
