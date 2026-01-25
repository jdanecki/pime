#include "alchemist/random_functions.h"
#include <cstdlib>
#include <cstring>

char vowels[] = "euyioa";
char consonant[] = "qwrtpsdfghjklzxcvbnm";

char * create_name(int s)
{
    // solid 4 syllables
    // liquid 3 syllables
    // gas 2 syllables
    // being 5 syllables
    int v = strlen(vowels);
    int c = strlen(consonant);

    char * n = new char[1 + 2 * s];
    for (int i = 0; i < s; i++)
    {
        n[2 * i] = consonant[random_range(0, c - 1)];
        n[2 * i + 1] = vowels[random_range(0, v - 1)];
    }
    n[2 * s] = 0;
    return n;
}
