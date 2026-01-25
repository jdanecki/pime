#include "dimensions.h"

Dimensions::Dimensions() : length("length", 1, 2), width("width", 1, 2), height("height", 1, 2), volume("volume", length.value * width.value * height.value)
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