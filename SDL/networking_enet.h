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
        Scroll,
        Player,
        Npc,
        Place,
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
    struct Place_Body
    {
        Place data;
    };

    ObjectData(Tag tag, size_t s) : tag(tag), size(s)
    {
        CONSOLE_LOG("ObjectData: set size to %ld\n", size);
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
        return ::operator new(size);
    }
    static void * operator new(size_t size_base, size_t extra)
    {
        CONSOLE_LOG("ObjectData: serial_data: allocating %ld + %ld\n", size_base, extra);
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
        Place_Body place;
    };
    int id;
    unsigned char data[0];
};

struct LocationUpdateData
{
    NetworkObject id;
    ItemLocation old;
    ItemLocation new_;
};

NetClient * init(const char * server_ip, const char * port);

uint32_t network_tick(NetClient * client);

InventoryElement *get_object_by_id(NetworkObject uid);

void register_object(InventoryElement * o, void *);

void deregister_object(InventoryElement * o);

BaseElement * get_base_element(size_t id);

BasePlant * get_base_plant(size_t id);

BaseAnimal * get_base_animal(size_t id);

Base * get_base(uint32_t c_id, int32_t id);

extern void update_player(uintptr_t id, int32_t map_x, int32_t map_y, int32_t x, int32_t y, int32_t thirst, int32_t hunger);

extern void update_chunk(int32_t x, int32_t y, const chunk_table * data);

extern void got_id(size_t id, int64_t seed);

extern void update_object(const ObjectData * data);

extern void update_item_location(LocationUpdateData data);

extern void create_object(const ObjectData * data);

extern void destroy_object(NetworkObject id, ItemLocation location);

extern void failed_craft();

extern void action_failed();

extern void knowledge_update(int32_t pl_id, Class_id cid, int32_t id);

extern void checked_update(int32_t pl_id, uintptr_t el);

void send_packet_move(NetClient * client, int32_t x, int32_t y);

void send_packet_pickup(NetClient * client, uintptr_t id);

void send_packet_drop(NetClient * client, uintptr_t id);

void send_packet_item_used_on_object(NetClient * client, uintptr_t iid, uintptr_t oid);

void send_packet_action_on_object(NetClient * client, int32_t a, uintptr_t oid);

void send_packet_server_action_on_object(NetClient * client, int32_t a, uintptr_t oid);

void send_packet_item_used_on_tile(NetClient * client, uintptr_t iid, ItemLocation location);

void send_packet_craft(NetClient * client, uintptr_t prod_id, uintptr_t ingredients_num, const uintptr_t * iid);

void send_packet_request_chunk(NetClient * client, int32_t x, int32_t y);

void send_packet_request_item(NetClient * client, size_t id);

ObjectData foo();
