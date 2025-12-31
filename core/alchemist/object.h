#ifndef OBJECT_H
#define OBJECT_H

#include <cstddef>

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
    Class_Place,

    Class_Plant,  // base: BasePlant -> Base
    Class_Animal, // base: BaseAnimal->Base
    Class_Player,
    Class_Npc,

    Class_Clan,

    Class_ListElement,
    Class_KnownElement,
    Class_BaseListElement,
};

class NetworkObject
{
  public:
    NetworkObject(Class_id id);
    NetworkObject(Class_id id, size_t uid);
    NetworkObject() {}
    size_t get_uid() const
    {
        return uid;
    }

    Class_id c_id;
    size_t uid;
};

#endif
