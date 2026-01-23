#include <cstring>

#include "serialization.h"

SerializableCString::SerializableCString(const char * s)
{
    strncpy(str, s, sizeof(str) - 1);
    str[sizeof(str) - 1] = 0;
}
SerializableCString::SerializableCString()
{
}
