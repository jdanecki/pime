#ifndef ELEMENTS_SERVER_H
#define ELEMENTS_SERVER_H

#include "../core/alchemist/elements.h"

void to_bytes_binding(InventoryElement * el, unsigned char * buf);
unsigned int get_packet_size_binding(InventoryElement * el);

void notify_update(const InventoryElement * el);
void notify_create(const InventoryElement * el);
void update_location(NetworkObject id, ItemLocation old_loc, ItemLocation new_loc);
void notify_destroy(NetworkObject id, ItemLocation location);
void notify_knowledge(size_t pl_id, Class_id cid, int id);
void notify_checked(size_t pl_id, size_t el);

void destroy(InventoryElement * el);

const unsigned long TICK_DELAY = 100;

#include "elements/animal_server.h"     // IWYU pragma: export
#include "elements/being_server.h"      // IWYU pragma: export
#include "elements/element_server.h"    // IWYU pragma: export
#include "elements/ingredient_server.h" // IWYU pragma: export
#include "elements/plant_server.h"      // IWYU pragma: export
#include "elements/product_server.h"    // IWYU pragma: export
#include "elements/scroll_server.h"     // IWYU pragma: export

#endif
