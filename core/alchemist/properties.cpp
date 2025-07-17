#include "properties.h"
#include <stdlib.h>

SerializableCString::SerializableCString(const char* ptr) : str(ptr) {}

Edible::Edible()
{
    caloric = new Property("caloric", rand() % 1000);
    ;
    irrigation = new Property("irrigation", rand() % 500);
    poison = new Property("poison", 0);
    if (rand() % 100 < 10)
    { // 10%
        poison->value = 1 + rand() % 250;
    }
}

Edible::~Edible()
{
    delete irrigation;
    delete poison;
    delete caloric;
}

void Edible::show()
{
    printf("      *** Edible ***\n");
    caloric->show();
    irrigation->show();
    poison->show();
}

Solid::Solid()
{
    tooling = new Property("tooling", 1 + rand() % 100);
    //1 - trudna - potrzebne narzędzia
    //100 - łatwe - obróbka ręczna
/*
 *  stal sztabka	niska
    kamień	niska
    drewno	średnia
    mięso	średnia
    kauczuk	średnia
    pajęczyna	wysoka
    glina	wysoka
  */
    stretching = new Property("stretching", 1 + rand() % 10000);
    //wytrzymałość
    // styropian: 1
    // beton:     5
    // cyna:     14
    // drewno:   800
    // diament: 1800
    // żelazo:  3800
    // stal:   10000
    /*
        kamień	niska - kruszy się
        glina	niska
        mięso	niska
        drewno	średnia
        kauczuk	średnia
        pajęczyna	wysoka
        stal sztabka	wysoka
      */


    squeezing = new Property("squezzing", 1 + rand() % 20000);
    //wytrzymałość
    // styropian:     1
    // beton:        50
    // kość:        150
    // kamień: 100-5000
    // diament:   17000
    /*
        kamień	niska
        stal sztabka	niska
        kauczuk	średnia
        drewno	średnia
        glina	wysoka
        mięso	wysoka
        pajęczyna wysoka
      */

    //na bazie wytrzymałości materiału
    //fragility = new Property("fragility", 1000 * stretching->value / squeezing->value);
    // < 100 kruche
    // > spręzyste


    bending = new Property("bending", 1 + rand() % 100);
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


    solubility = new Property("solubility", 1 + rand() % 100);
    //rozpuszczalność

    hardness = new Property("hardness", 1 + rand() % 100);
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
    delete stretching;
    delete squeezing;
    delete bending;
   // delete fragility;
    delete solubility;
}

void Solid::show()
{
    printf("      *** Solid ***\n");

    stretching->show(); // rozciąganie
    squeezing->show();  // ściskanie
    bending->show();    // zginanie
    //fragility->show();  // kruchość
    solubility->show(); // rozpuszczalność
}


const char * Form_name[]{"unknown", "solid", "liquid", "gas"};
