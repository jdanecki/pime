#ifndef __ELEMENTS_BASE_H
#define __ELEMENTS_BASE_H

#include "../object.h"
#include "../names.h"
#include "../properties.h"

class Base : public NetworkObject
{
  public:
    Edible edible;

    SerializableCString name;
    Base(int index, Class_id c, const char * name);
    virtual ~Base();
    virtual void show(bool details = true);
    const char * get_name();
    virtual size_t get_size();
    virtual void copy_data(unsigned char * ptr, int i);
};

#endif
