#ifndef __ELEMENTS_BASE_ANIMAL_H
#define __ELEMENTS_BASE_ANIMAL_H

#include "base.h"

class BaseAnimal : public Base
{
  public:
    bool carnivorous; // mięsożerny
    bool swimming;
    bool flying;

    BaseAnimal(int index);
    void show(bool details = true) override;
    size_t get_size() override;
    void copy_data(unsigned char * ptr, int i) override;
};

#endif
