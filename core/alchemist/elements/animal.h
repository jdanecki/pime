#ifndef __ELEMENTS_ANIMAL_H
#define __ELEMENTS_ANIMAL_H

#include "inventory_element.h"
#include "base_animal.h"
#include "../object.h"

class Animal : public InventoryElement
{
    SerializablePointer<BaseAnimal> base;
    void init(BaseAnimal * b);

  public:
    bool wild;
    Animal(BaseAnimal * b);
    Animal(int id);
    void show(bool details = true) override;

    BaseAnimal * get_base();
    size_t get_id() override;

    Class_id get_base_cid() override;

    bool action(Product_action action, Player * pl) override;
    const char * get_name() override;
    char * get_description() override;
    ~Animal();
};

BaseAnimal * get_base_animal(size_t id);

#endif
