#include "../elements_server.h"

#define GENERATE_PRODUCT(name) \
class name : public ProductServer \
{ \
    name(InventoryElement * el1, InventoryElement * el2); \
  public: \
    static ProductServer * create##name(InventoryElement * el1, InventoryElement * el2); \
}; \

#define GENERATE_INGREDIENT(name) \
class name : public IngredientServer \
{ \
    name(InventoryElement * from); \
  public: \
    static IngredientServer * create##name(InventoryElement * from); \
}; \

#define GENERATE_TOOL(name) \
GENERATE_PRODUCT(name) \
GENERATE_INGREDIENT(name##Blade) \
GENERATE_INGREDIENT(name##Handle) \

GENERATE_TOOL(Axe)
GENERATE_TOOL(Knife)
GENERATE_TOOL(Pickaxe)

GENERATE_PRODUCT(Fire)
GENERATE_INGREDIENT(Stick)
GENERATE_INGREDIENT(Tinder)

GENERATE_PRODUCT(RoastedMeat)
GENERATE_INGREDIENT(Meat)

GENERATE_PRODUCT(FruitSalad)
GENERATE_INGREDIENT(Fruit)

GENERATE_PRODUCT(Hut)
GENERATE_INGREDIENT(Log)
GENERATE_INGREDIENT(Wall)
