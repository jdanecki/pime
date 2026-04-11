#ifndef __ELEMENTS_PLACE_H
#define __ELEMENTS_PLACE_H

#include "types.h"

class Place
{
  public:
    Place_id id;
    const char * get_place_name();
    Place(Place_id id);
    virtual ~Place() {}
    void show(bool details = true);
    char * get_description();
    virtual void show_state();
};

#endif
