#ifndef PROPERTIES_H
#define PROPERTIES_H

#include <stdio.h>
#include <stdlib.h>
#include "serialization.h"
#include "ncurses-output.h"

class Property
{
  public:
    SerializableCString name;
    unsigned int value;
    Property(const char * n, unsigned int v) : name(n)
    {
        value = v;
    }
#ifdef USE_ENET
    Property()
    {
    }
#endif
    void show()
    {
        CONSOLE_LOG("%s = %u\n", name.str, value);
    }
    unsigned int decrease(unsigned int by)
    {
        if (value > by)
        {
            value -= by;
        }
        else
            value = 0;
        return value;
    }
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
    void set_random()
    {
        eating_by = rand() % 16;
    }
    void set_no_edible()
    {
        eating_by = 0;
    }
    bool can_be_eaten_by(int who)
    {
        return eating_by & (1 << who);
    }
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

enum Form
{
    Form_unknown = 0,
    Form_solid = 1,
    Form_liquid,
    Form_gas,
};

extern const char * Form_name[];

#endif // PROPERTIES_H
