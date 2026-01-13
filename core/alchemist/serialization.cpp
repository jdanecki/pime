#include "serialization.h"
#include <cstring>

SerializableCString::SerializableCString(const char * s)
{
    strncpy(str, s, sizeof(str) - 1);
    str[sizeof(str) - 1] = 0;
}
