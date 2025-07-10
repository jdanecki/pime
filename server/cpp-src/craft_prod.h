#ifndef CRAFT_PROD_H
#define CRAFT_PROD_H

#include "../../core/alchemist/elements.h"
#include "tools/axe.h"
#include "tools/pickaxe.h"
#include "tools/knife.h"
#include "tools/hut.h"

InventoryElement * craft_prod(int product_id, InventoryElement * el1, InventoryElement * el2);

#endif

