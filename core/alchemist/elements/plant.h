#ifndef __ELEMENTS_PLANT_H
#define __ELEMENTS_PLANT_H

#include "inventory_element.h"
#include "base_plant.h"
#include "../object.h"

class Plant : public InventoryElement
{
    SerializablePointer<BasePlant> base;
    void init(BasePlant * b);

  protected:
    unsigned int seedling_time;
    unsigned int growing_time;
    unsigned int flowers_time;

  public:
    int water;

    Plant_phase phase;
    bool grown;

    Plant(BasePlant * b);
    Plant(int id);
    void show(bool details = true) override;
    BasePlant * get_base();
    const char * get_name() override;
    size_t get_id() override;
    Class_id get_base_cid() override;
    bool action(Product_action action, Player * pl) override;

    char * get_description() override;
    Property ** get_properties(int * count) override;
};

BasePlant * get_base_plant(size_t id);

#endif
