#ifndef NETWORKING_CORE_H
#define NETWORKING_CORE_H

#include "../../core/alchemist/el_list.h"

extern InvList objects_to_update;
extern InvList objects_to_update_reliable;

extern InvList objects_to_create;

class Networked
{
    public:
    Networked();
};

#endif
