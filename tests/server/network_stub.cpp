#ifdef TESTS
#include "network_stub.h"
#include "../../server/networking.h"

void notify_destroy(InventoryElement * el)
{
}
void notify_create(const InventoryElement * el)
{
}
void notify_update(const InventoryElement * el)
{
}
void update_location(NetworkObject id, ItemLocation old_loc, ItemLocation new_loc)
{
}
void notify_knowledge(size_t pl_id, Class_id cid, int id)
{
}

Networked::Networked()
{
}
#endif
