#ifndef DIMENSIONS_H
#define DIMENSIONS_H

#include "../properties.h"

class Dimensions
{
  public:
    float max_width;
    float max_height;
    float max_length;

    Property length;
    Property width;
    Property height;
    Property volume; // lenght*width*height
    Dimensions(float min, float max);
    Dimensions();
    void show(bool details = true);
    void scale(float by);
};

#endif
