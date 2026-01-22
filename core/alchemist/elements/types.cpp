#include "types.h"

const char * ingredient_name[] = {
    "Axe blade",
    "Axe handle",

    "Knife blade",
    "Knife handle",

    "Pickaxe blade",
    "Pickaxe handle",

    "Hoe blade",
    "Hoe handle",

    "Wall",
    "Meat",

    "Log",
    "Tinder",
    "Stick",
    "Fruit",
    "Seed",

};

const char * product_name[] = {
    "Axe",
    "Knife",
    "Pickaxe",
    "Hoe",
    "Hut",
    "Tent",
    "Fire",
    "Roasted meat",
    "Fruit Salad",
    "Seedling",
    "Feed",
};

const char * product_action_name[] = {
    "nothing",
    "cut",
    "hit",
    "stab",
    "fire",
    "plow",
    "plant",
    "invite",
    "feed",
};
const char * player_action_name[] = {"drink", "eat", "read", "check"};

const char * server_action_name[] = {"server show item", "server show chunk", "server trace network"};

const char * places_names[] = {"field", "barn"};
const char * field_states_names[] = {"plowed", "planted"};
const char * barn_states_names[] = {"empty", "animal", "full"};

const char * plant_phase_name[] = {"Seedling", "Growing", "Flowers", "Fruits"};
const char * class_name[] = {"unknown", "BaseElement", "BaseAnimal", "BasePlant", "Element", "Ingredient", "Product", "Scroll", "Place", "Plant", "Animal", "Player", "Npc", "Clan", "ListElement",
    "KnownElement", "BaseListElement"};
