#include "../elements_server.h"
#include "../networking.h"

#ifndef TOOLS_H
#define TOOLS_H

#define GENERATE_PRODUCT(name, body)                                                                                                                                                                   \
    class name : public ProductServer, public Networked                                                                                                                                                \
    {                                                                                                                                                                                                  \
        name(InventoryElement * el1, InventoryElement * el2);                                                                                                                                          \
                                                                                                                                                                                                       \
      public:                                                                                                                                                                                          \
        static ProductServer * create##name(InventoryElement * el1, InventoryElement * el2);                                                                                                           \
        body                                                                                                                                                                                           \
    };

#define GENERATE_INGREDIENT(name, body)                                                                                                                                                                \
    class name : public IngredientServer, public Networked                                                                                                                                             \
    {                                                                                                                                                                                                  \
        name(InventoryElement * from);                                                                                                                                                                 \
                                                                                                                                                                                                       \
      public:                                                                                                                                                                                          \
        static IngredientServer * create##name(InventoryElement * from);                                                                                                                               \
        body                                                                                                                                                                                           \
    };

#define GENERATE_TOOL(name)                                                                                                                                                                            \
    GENERATE_PRODUCT(name, )                                                                                                                                                                           \
    GENERATE_INGREDIENT(name##Blade, )                                                                                                                                                                 \
    GENERATE_INGREDIENT(name##Handle, )

GENERATE_TOOL(Axe)
GENERATE_TOOL(Knife)
GENERATE_TOOL(Pickaxe)

GENERATE_PRODUCT(Fire, )
GENERATE_INGREDIENT(Stick, )
GENERATE_INGREDIENT(Tinder, )

GENERATE_PRODUCT(RoastedMeat, bool player_action(Player_action, Player *);)
GENERATE_INGREDIENT(Meat, bool action(Product_action, Player *);)

GENERATE_PRODUCT(FruitSalad, bool player_action(Player_action, Player *);)
GENERATE_INGREDIENT(Fruit, )

GENERATE_PRODUCT(Hut, )
GENERATE_INGREDIENT(Log, )
GENERATE_INGREDIENT(Wall, )

#endif
