#ifndef OBJECT_H
#define OBJECT_H

#include <cstddef>
// FIXME @jacek add enet
#include "serialization-rust.h"

enum Class_id
{
    Class_Unknown = 0,
    Class_BaseElement,
    Class_BaseAnimal,
    Class_BasePlant,

    Class_Element, // base: BaseElement
    Class_Ingredient,
    Class_Product,
    Class_Scroll,

    Class_Plant,  // base: BasePlant -> Base
    Class_Animal, // base: BaseAnimal->Base
    Class_Player,
    Class_Npc,

    Class_Clan,
};

class NetworkObject
{
public:
    NetworkObject(Class_id id);
    NetworkObject(Class_id id, size_t uid);
    Class_id c_id;
    size_t uid;
};



#endif
