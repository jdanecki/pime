#include "networking.h"
#include "elements_server.h"

Networked::Networked()
{
    notify_create((InventoryElement *)this);
}
