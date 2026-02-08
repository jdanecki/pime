#ifndef BEING_SERVER_H
#define BEING_SERVER_H

#include "../../core/alchemist/properties.h"
#include "../../core/alchemist/elements/inventory_element.h"

const int max_delay_move = 1;   // 0.1 sec.
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
    ItemLocation dst_loc;

    BeingServer();
    void show(bool details = true);
    Property ** get_properties(int * count);
    ~BeingServer();
    virtual bool grow();
    virtual bool tick();
    bool check_move();
    void discover(InventoryElement * who);
    void move_by(InventoryElement * who, float dx, float dy);
    void move_to(InventoryElement * who);
};

#endif // BEING_SERVER_H
