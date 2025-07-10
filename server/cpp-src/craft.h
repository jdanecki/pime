#ifndef CRATFING_H
#define CRATFING_H

#include "elements_server.h"
#include "player_server.h"

bool craft(int product_id, int ingredients_num, const size_t * ingredients_ids, Player * player);

#endif
