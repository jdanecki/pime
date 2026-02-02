#ifndef BEING_SERVER_H
#define BEING_SERVER_H

#include "../../core/alchemist/properties.h"
#include "../../core/alchemist/item_location.h"

const int max_delay_move = 10;  // 1 sec.
const int max_delay_grow = 100; // 600; // 600=1 min.

class BeingServer
{
  public:
    Property * age;
    Property * max_age;
    bool alive;
    int padding;
    int delay_for_move;
    int delay_for_grow;
    float dst_loc_x, dst_loc_y;

    BeingServer();
    void show(bool details = true);
    Property ** get_properties(int * count);
    ~BeingServer();
    virtual bool grow();
    virtual bool tick();
    bool check_move();
};

void being_move(BeingServer * being, ItemLocation * location);
#endif // BEING_SERVER_H