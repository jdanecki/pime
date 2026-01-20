#ifndef __ELEMENTS_BASE_PLANT_H
#define __ELEMENTS_BASE_PLANT_H

#include "base.h"

class BasePlant : public Base
{
  public:
    bool flowers;
    bool leaves;
    BasePlant(int index);
    void show(bool details = true) override;
    size_t get_size() override;
    void copy_data(unsigned char * ptr, int i) override;
};

#endif
