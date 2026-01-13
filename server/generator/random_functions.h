#ifndef RANDOM_FUNCTIONS_H
#define RANDOM_FUNCTIONS_H
#include <cstddef>
#include <cstdlib>

int random_range(int min, int max);
float random_float_range(float min, float max);
int random_bool(double probability);
class chunk;
typedef void (*callback_fn)(chunk * ch, size_t id);
void do_times(float prob, callback_fn f, chunk * ch, int id);

#endif // RANDOM_FUNCTIONS_H
