#ifndef __ELEMENTS_INGREDIENT_H
#define __ELEMENTS_INGREDIENT_H

#include "inventory_element.h"

class Ingredient : public InventoryElement
{
  public:
    Property quality;    //[0..100] slaby..najlepszy
    Property resilience; // [0..100] wytrzymały..słaby
    Property usage;      // [0..100] łatwy..trudny

    Ingredient_id id;
    Form req_form;

    Property ** get_properties(int * count) override;
    size_t get_id() override;
    Ingredient(Ingredient_id i);

    void show(bool details = true) override;

    const char * get_name() override;
    char * get_description() override;
    bool action(Product_action action, Player * pl) override;
};

#endif
