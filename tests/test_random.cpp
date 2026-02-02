#include "../core/alchemist/random_functions.h"
#include <cstdio>
#include <ctime>

int main()
{
    srandom(time(NULL));    
    for (int i = 0; i < 10; i++)
    {
        printf("%d: %d %d\n", i, random_range(0, 1), i % 2);
    }
    for (int i = 0; i < 10; i++)
    {
        printf("%d: %d\n", i, random_range(-5, 5));
    }
    for (int i = 0; i < 10; i++)
    {
        printf("%d: %d\n", i, random_range(0, 10));
    }
    for (int i = 0; i < 10; i++)
    {
        printf("%d: %d\n", i, random_range(-5, 6));
    }
    for (int i = 0; i < 10; i++)
    {
        printf("%d: %d\n", i, random_range(1, 5));
    }
    return 0;

}   