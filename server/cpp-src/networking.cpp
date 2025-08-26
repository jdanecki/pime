#include "networking.h"

InvList objects_to_update;
InvList objects_to_update_reliable;

InvList objects_to_create;
InvList objects_to_destroy;

Networked::Networked()
{
    objects_to_create.add((InventoryElement*)this);
}
