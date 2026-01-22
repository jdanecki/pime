#ifndef __ELEMENTS_TYPES_H
#define __ELEMENTS_TYPES_H

enum Product_action
{
    ACT_NOTHING,
    ACT_CUT,
    ACT_HIT,
    ACT_STAB, // dźgnij
    ACT_FIRE,
    ACT_PLOW, // oraj
    ACT_PLANT,
    ACT_INVITE,
    ACT_FEED,
};

extern const char * product_action_name[];

enum Player_action
{
    PLAYER_DRINK,
    PLAYER_EAT,
    PLAYER_READ,
    PLAYER_CHECK,
};

extern const char * player_action_name[];

enum Server_action
{
    SERVER_SHOW_ITEM,
    SERVER_SHOW_CHUNK,
    SERVER_TRACE_NETWORK,
};

extern const char * server_action_name[];

struct ColorRGB
{
    unsigned char r;
    unsigned char g;
    unsigned char b;
};

enum Place_id
{
    PLACE_FIELD,
    PLACE_BARN,
    PLACES_COUNT,
};

extern const char * places_names[];

enum Field_states
{
    FIELD_PLOWED,
    FIELD_PLANTED,
};

enum Barn_states
{
    BARN_EMPTY,
    BARN_ANIMAL,
    BARN_FULL,
};

extern const char * field_states_names[];
extern const char * barn_states_names[];

enum Ingredient_id
{
    ING_AXE_BLADE,
    ING_AXE_HANDLE,

    ING_KNIFE_BLADE,
    ING_KNIFE_HANDLE,

    ING_PICKAXE_BLADE,
    ING_PICKAXE_HANDLE,

    ING_HOE_BLADE,
    ING_HOE_HANDLE,

    ING_WALL,

    ING_MEAT,

    ING_LOG,
    ING_TINDER,
    ING_STICK,

    ING_FRUIT,
    ING_SEED,

    ING_COUNT,

};

extern const char * ingredient_name[];

enum Product_id
{
    PROD_AXE,
    PROD_KNIFE,
    PROD_PICKAXE,
    PROD_HOE,
    PROD_HUT,
    PROD_TENT,
    PROD_FIRE,
    PROD_ROASTED_MEAT,
    PROD_FRUIT_SALAD,
    PROD_SEEDLING,
    PROD_FEED,
    PROD_COUNT
};

extern const char * product_name[];

enum Plant_phase
{
    Plant_seedling,
    Plant_growing,
    Plant_flowers,
    Plant_fruits
};

extern const char * plant_phase_name[];

extern const char * class_name[];

#endif
