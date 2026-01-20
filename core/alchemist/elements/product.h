#ifndef __ELEMENTS_PRODUCT_H
#define __ELEMENTS_PRODUCT_H

#include "inventory_element.h"

class Product : public InventoryElement
{
  public:
    Property quality;    //[0..100] slaby..najlepszy
    Property resilience; // [0..100] wytrzymały..słaby
    Property usage;      // [0..100] łatwy..trudny
    Form req_form;

    Product_action actions[10];
    int actions_count;

    Product_id id;
    size_t get_id() override;
    Product(Product_id id, int actions_count);
    virtual void add_action(Product_action * a);
    Property ** get_properties(int * count) override;
    virtual bool check_ing();
    void show(bool details = true) override;

    const char * get_name() override;
    char * get_description() override;
};

#endif
