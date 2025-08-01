#include <cstdarg>
#include <cstdint>
#include <cstdlib>
#include <ostream>
#include <new>

template <typename K = void, typename V = void, typename Hasher = void> struct HashMap;

template <typename T = void> struct Vec;

struct ObjectData
{
    enum class Tag
    {
        InvElement,
        Element,
        Plant,
        Animal,
        Ingredient,
        Product,
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

    Tag tag;
    union
    {
        InvElement_Body inv_element;
        Element_Body element;
        Plant_Body plant;
        Animal_Body animal;
        Ingredient_Body ingredient;
        Product_Body product;
    };
};

struct LocationUpdateData
{
    uintptr_t id;
    ItemLocation old;
    ItemLocation new_;
};

extern "C"
{

    NetClient * init(const char * server_ip, const char * port);

    void network_tick(NetClient * client);

    BaseElement * get_base_element(int32_t id);

    BasePlant * get_base_plant(int32_t id);

    BaseAnimal * get_base_animal(int32_t id);

    extern void update_player(uintptr_t id, int32_t map_x, int32_t map_y, int32_t x, int32_t y, int32_t thirst, int32_t hunger);

    extern void update_chunk(int32_t x, int32_t y, const chunk_table * data);

    extern void got_id(uintptr_t id, int64_t seed);

    extern void update_object(const ObjectData * data);

    extern void update_item_location(LocationUpdateData data);

    extern void create_object(const ObjectData * data);

    extern void destroy_object(uintptr_t id, ItemLocation location);

    extern void failed_craft();

    extern void action_failed();

    void send_packet_move(NetClient * client, int32_t x, int32_t y);

    void send_packet_pickup(NetClient * client, uintptr_t id);

    void send_packet_drop(NetClient * client, uintptr_t id);

    void send_packet_item_used_on_object(NetClient * client, uintptr_t iid, uintptr_t oid);

    void send_packet_action_on_object(NetClient * client, int32_t a, uintptr_t oid);

    void send_packet_server_action_on_object(NetClient * client, int32_t a, uintptr_t oid);

    void send_packet_item_used_on_tile(NetClient * client, uintptr_t iid, int32_t map_x, int32_t map_y, int32_t x, int32_t y);

    void send_packet_craft(NetClient * client, uintptr_t prod_id, uintptr_t ingredients_num, const uintptr_t * iid);

    void send_packet_request_chunk(NetClient * client, int32_t x, int32_t y);

    ObjectData foo();

} // extern "C"
