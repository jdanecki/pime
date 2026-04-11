#include "../../core/alchemist/elements.h"
#include "../networking.h"

#ifndef PLACES_H
#define PLACES_H

class FieldServer : public Field, public Networked
{
  public:
    FieldServer(Place_id id);
    static InventoryElement * createField(Place_id id);

    void show_state() override;
    bool action(Product_action, Player *) override;
    bool can_pickup() override;
};

class BarnServer : public Barn, public Networked
{
  public:
    BarnServer(Place_id id);
    static InventoryElement * createBarn(Place_id id);

    void show_state() override;
    bool action(Product_action, Player *) override;
    bool can_pickup() override;
    bool use_on(InventoryElement * object, Player * pl);
};

InventoryElement * create_place(Place_id id);
#endif
