#include "properties.h"
#include <stdlib.h>
#include "../clan.h"
#include <cstring>

Edible::Edible() : irrigation("irrigation", rand() % 500), poison("poison", 1 + rand() % 250), caloric("caloric", rand() % 1000)
{
    set_random();
}

Edible::~Edible()
{
}

void Edible::show()
{
    CONSOLE_LOG("      *** Edible ***\n");
    CONSOLE_LOG("eating by: %x -> ", eating_by);
    for (int i = 0; i < 5; i++)
    {
        if (eating_by & (1 << i))
            CONSOLE_LOG("%s ", clan_names[i]);
    }
    CONSOLE_LOG("\n");
    caloric.show();
    irrigation.show();
    poison.show();
}

Solid::Solid()
    : tooling("tooling", 1 + rand() % 100), stretching("stretching", 1 + rand() % 10000), squeezing("squezzing", 1 + rand() % 20000), bending("bending", 1 + rand() % 100),
      solubility("solubility", 1 + rand() % 100), hardness("hardness", 1 + rand() % 100)
{
    // tooling
    //  1 - trudna - potrzebne narzędzia
    //  100 - łatwe - obróbka ręczna
    /*
     *  stal sztabka	niska
        kamień	niska
        drewno	średnia
        mięso	średnia
        kauczuk	średnia
        pajęczyna	wysoka
        glina	wysoka
      */
    // stretching
    //  wytrzymałość
    //   styropian: 1
    //   beton:     5
    //   cyna:     14
    //   drewno:   800
    //   diament: 1800
    //   żelazo:  3800
    //   stal:   10000
    /*
        kamień	niska - kruszy się
        glina	niska
        mięso	niska
        drewno	średnia
        kauczuk	średnia
        pajęczyna	wysoka
        stal sztabka	wysoka
      */

    // squeezing
    //  wytrzymałość
    //   styropian:     1
    //   beton:        50
    //   kość:        150
    //   kamień: 100-5000
    //   diament:   17000
    /*
        kamień	niska
        stal sztabka	niska
        kauczuk	średnia
        drewno	średnia
        glina	wysoka
        mięso	wysoka
        pajęczyna wysoka
      */

    // na bazie wytrzymałości materiału
    // fragility = new Property("fragility", 1000 * stretching->value / squeezing->value);
    //  < 100 kruche
    //  > spręzyste

    // bending = new Property("bending", 1 + rand() % 100);
    // 1 trudno  zginalne
    // 100 łatwo
    /*
     *  stal sztabka	niska
        kamień	niska
        drewno	średnia
        glina	średnia
        kauczuk	średnia
        mięso	wysoka
        pajęczyna	wysoka
      */

    // solubility = new Property("solubility", 1 + rand() % 100);
    //  rozpuszczalność

    // hardness = new Property("hardness", 1 + rand() % 100);
    /*
        kamień	wysoka
        drewno	średnia
        glina	niska
        mięso	niska
        kauczuk	średnia
        pajęczyna	niska
        stal sztabka	wysoka
      */
}

Solid::~Solid()
{
}

void Solid::show()
{
    CONSOLE_LOG("      *** Solid ***\n");

    tooling.show();
    stretching.show(); // rozciąganie
    squeezing.show();  // ściskanie
    bending.show();    // zginanie
    // fragility->show();  // kruchość
    solubility.show(); // rozpuszczalność
    hardness.show();
}

const char * Form_name[]{"unknown", "solid", "liquid", "gas"};
