#include "dimensions.h"
#include "../random_functions.h"

Dimensions::Dimensions(float min, float max)
    : max_width(random_float_range(min, max)), max_height(random_float_range(min, max)), max_length(random_float_range(min, max)), length("length", min, max_length), width("width", min, max_width),
      height("height", min, max_height), volume("volume", length.value * width.value * height.value)
{
}
Dimensions::Dimensions()
{
}

void Dimensions::show(bool details)
{
    if (details)
    {
        length.show();
        width.show();
        height.show();
        volume.show();
    }
    else
    {
        CONSOLE_LOG("length=%0.1f width=%0.1f height=%0.1f volume=%0.0f\n", length.value, width.value, height.value, volume.value);
    }
}

void Dimensions::scale(float by)
{ // by is 0-1
    length.value = max_length * by;
    width.value = max_width * by;
    height.value = max_height * by;
    volume.value = length.value * width.value * height.value;
}
