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

void do_times(float prob, callback_fn f, chunk * ch, int id)
{
    int count = (int)(prob * 2.0f);
    for (int i = 0; i < count; ++i)
    {
        if (random_bool(0.5))
        {
            f(ch, id);
        }
    }
}
