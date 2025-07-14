//GENERATED FILE
#ifndef NETWORKING_H
#define NETWORKING_H
class UdpSocket {};
#include "../core/world.h"
#include <cstdarg>
#include <cstdint>
#include <cstdlib>
#include <ostream>
#include <new>

struct NetClient {
  UdpSocket socket;
};

struct ObjectData {
  enum class Tag {
    InvElement,
    Element,
    Plant,
    Animal,
    Ingredient,
    Product,
  };

  struct InvElement_Body {
    InventoryElement data;
  };

  struct Element_Body {
    Element data;
  };

  struct Plant_Body {
    Plant data;
  };

  struct Animal_Body {
    Animal data;
  };

  struct Ingredient_Body {
    Ingredient data;
  };

  struct Product_Body {
    Product data;
  };

  Tag tag;
  union {
    InvElement_Body inv_element;
    Element_Body element;
    Plant_Body plant;
    Animal_Body animal;
    Ingredient_Body ingredient;
    Product_Body product;
  };
};

struct LocationUpdateData {
  uintptr_t id;
  ItemLocation old;
  ItemLocation new_;
};

extern "C" {

const NetClient *init();

void network_tick(const NetClient *client);

BaseElement *get_base_element(int32_t id);

BasePlant *get_base_plant(int32_t id);

BaseAnimal *get_base_animal(int32_t id);

extern void update_player(uintptr_t id, int32_t map_x, int32_t map_y, int32_t x, int32_t y);

extern void update_chunk(int32_t x, int32_t y, const chunk_table *data);

extern void got_id(uintptr_t id, int64_t seed);

extern void update_object(ObjectData data);

extern void update_item_location(LocationUpdateData data);

extern void create_object(const ObjectData *data);

extern void destroy_object(uintptr_t id, ItemLocation location);

extern void failed_craft();

void send_packet_move(const NetClient *client, int32_t x, int32_t y);

void send_packet_pickup(const NetClient *client, uintptr_t id);

void send_packet_drop(const NetClient *client, uintptr_t id);

void send_packet_item_used_on_object(const NetClient *client, uintptr_t iid, uintptr_t oid);

void send_packet_item_used_on_tile(const NetClient *client,
                                   uintptr_t iid,
                                   int32_t map_x,
                                   int32_t map_y,
                                   int32_t x,
                                   int32_t y);

void send_packet_craft(const NetClient *client,
                       uintptr_t prod_id,
                       uintptr_t ingredients_num,
                       const uintptr_t *iid);

void send_packet_request_chunk(const NetClient *client, int32_t x, int32_t y);

ObjectData foo();

}  // extern "C"
#endif
