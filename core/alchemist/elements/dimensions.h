#ifndef DIMENSIONS_H
#define DIMENSIONS_H

#include "../properties.h"

class Dimensions
{
  public:
    Property length;
    Property width;
    Property height;
    Property volume; // lenght*width*height
    Dimensions();
    void show(bool details = true);
};

#endif
