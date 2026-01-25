#include "random_functions.h"

int random_range(int min, int max)
{
    return rand() % (max - min) + min;
}

float random_float_range(float min, float max)
{
    return (float)rand() / (1.0 * RAND_MAX) * (max - min) + min;
}

int random_bool(double probability)
{
    return ((double)rand() / RAND_MAX) < probability;
}
