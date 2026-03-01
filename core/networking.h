#ifndef NETWORKING_H
#define NETWORKING_H

#include <cstdarg>
// #include <cstdint>
// #include <ostream>
#include <new>
#include <enet/enet.h>

#include "alchemist/elements.h"
#include "world.h"
#include "npc.h"

struct ObjectData
{
    enum class Tag
    {
        InvElement, // 0
        Element,
        Plant,
        Animal,
        Ingredient,
        Product, // 5
        Scroll,
        Player,
        Npc,
        Field,
        Barn,
        Clan,
    };

    struct InvElement_Body
    {
        InventoryElement data;
    };

    struct Element_Body
    {
        Element data;
    };

    struct Plant_Body
    {
        Plant data;
    };

    struct Animal_Body
    {
        Animal data;
    };

    struct Ingredient_Body
    {
        Ingredient data;
    };

    struct Product_Body
    {
        Product data;
    };

    struct Scroll_Body
    {
        Scroll data;
    };

    struct Player_Body
    {
        Player data;
    };

    struct Npc_Body
    {
        Npc data;
    };
    struct Field_Body
    {
        Field data;
    };
    struct Barn_Body
    {
        Barn data;
    };
    struct Clan_Body
    {
        Clan data;
    };

    ObjectData(Tag tag, size_t s) : tag(tag), size(s)
    {
     //   CONSOLE_LOG("ObjectData: set size to %ld\n", size);
    }
    ObjectData(Tag tag) : tag(tag)
    {
        size = sizeof(struct ObjectData);
        //    CONSOLE_LOG("ObjectData: size=%ld\n", size);
    }

    ~ObjectData()
    {
    }
    static void * operator new(size_t size)
    {
       // CONSOLE_LOG("ObjectData: new size=%ld\n", size);
        return ::operator new(size);
    }
    static void * operator new(size_t size_base, size_t extra)
    {
      //  CONSOLE_LOG("ObjectData: allocating %ld + %ld\n", size_base, extra);
        return ::operator new(size_base + extra);
    }

    static void operator delete(void * ptr)
    {
        ::operator delete(ptr);
    }

    Tag tag;
    size_t size;
    union
    {
        InvElement_Body inv_element;
        Element_Body element;
        Plant_Body plant;
        Animal_Body animal;
        Ingredient_Body ingredient;
        Product_Body product;
        Scroll_Body scroll;
        Player_Body player;
        Npc_Body npc;
        Field_Body field;
        Barn_Body barn;
        Clan_Body clan;
    };
    size_t id; //element (base), field, barn, plant (base), animal(base)
    unsigned char data[0];
};

struct LocationUpdateData
{
    NetworkObject id;
    ItemLocation old;
    ItemLocation new_;
};

#endif
