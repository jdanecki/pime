#ifndef PROPERTIES_H
#define PROPERTIES_H

#include <cstdio>

class Property
{
  public:
    const char * name;
    unsigned int value;
    Property(const char * n, unsigned int v)
    {
        name = n;
        value = v;
    }
    void show()
    {
        printf("%s = %u\n", name, value);
    }
};

class Edible
{ // FIXME should be changed to being property
  // when more npc's are added
  // can be eaten by humans or animals
  public:
    Property * irrigation;
    Property * poison;
    Property * caloric;

    Edible();
    ~Edible();
    void show();
};

class Solid
{
  public:
    Property * stretching; // <-->
    Property * squeezing;  //  >--<
    Property * bending;
    Property * fragility;  // kruchosc
    Property * solubility; // rozpuszczalnosc
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
