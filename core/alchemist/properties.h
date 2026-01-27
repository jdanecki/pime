#ifndef PROPERTIES_H
#define PROPERTIES_H

#include <stdlib.h>
#include "serialization.h"

class Property
{
  public:
    SerializableCString name;
    float value;
    Property(const char * n, float v);
    Property(const char * n, float min, float max);
    Property();
    void show();
    float decrease(float by);
};

class Edible
{
  public:
    Property irrigation;
    Property poison;
    Property caloric;
    int eating_by; // bit value 1 means edible for clanId, we use bits 0-4

    Edible();
    ~Edible();
    void set_random();
    void set_no_edible();
    bool can_be_eaten_by(int who);
    void show();
};

class Solid
{
  public:
    Property tooling;    // obróbka
    Property stretching; // rozciągalność
    Property squeezing;  // ściskalność
    Property bending;    // elastyczność
    //  Property fragility;  // kruchosc
    Property solubility; // rozpuszczalnosc
    Property hardness;   // twardość
    Solid();
    ~Solid();
    void show();
};

extern const char * Form_name[];

#endif // PROPERTIES_H
