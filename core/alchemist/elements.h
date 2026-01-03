#ifndef __ELEMENTS__H
#define __ELEMENTS__H

#include "object.h"
#include "names.h"
#include <cstddef>
#include <cstdlib>
#include <stdint.h>
#include "properties.h"
#include "item_location.h"
#include "serialization.h"

extern const char * class_name[];

enum Product_action
{
    ACT_NOTHING,
    ACT_CUT,
    ACT_HIT,
    ACT_STAB, // dźgnij
    ACT_FIRE,
    ACT_PLOW, //oraj
    ACT_PLANT,
    ACT_INVITE,
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

class Base: public NetworkObject
{
  public:
    Edible edible;

    SerializableCString name;
    Base(int index, Class_id c, const char * name);
    virtual ~Base()
    {
    }
    virtual void show(bool details = true);
    const char * get_name();
    virtual size_t get_size()
    {
        return sizeof(Base);
    }
};

struct Color
{
    int r;
    int g;
    int b;
};

class BaseElement : public Base
{
  public:
    Form form; // solid, liquid, gas
    Color color; //color for tile
    Property density;
    Solid solid;

    BaseElement(Form f, int index);
    void show(bool details = true);
    virtual size_t get_size()
    {
        return sizeof(BaseElement);
    }
};

class chunk;
class Player;

class InventoryElement : public NetworkObject
{
    // int x, y, z;
  public:
    ItemLocation location;

    InventoryElement(Class_id c_id, size_t uid, ItemLocation location);
    InventoryElement(Class_id c_id, size_t uid) : NetworkObject(c_id, uid)
    {
    }
    InventoryElement(Class_id c_id) : NetworkObject(c_id)
    {
    }
    InventoryElement()
    {
    }
    virtual bool action(Product_action action, Player * pl)
    {
        CONSOLE_LOG("INV: %s %s\n", product_action_name[action], get_name());
        return false;
    }
    virtual bool player_action(Player_action action, Player * pl)
    {
        CONSOLE_LOG("INV: %s %s\n", player_action_name[action], get_name());
        return false;
    }
    virtual void show(bool details = true)
    {
        CONSOLE_LOG("%s: uid=%lx id=%d @[%d,%d][%d,%d]\n", get_class_name(), uid, get_id(), location.chunk.map_x, location.chunk.map_y, location.chunk.x, location.chunk.y);
    }
    virtual bool tick()
    {
        return false;
    }
    virtual const char * get_name()
    {
        return nullptr;
    }
    const char * get_class_name()
    {
        return class_name[c_id];
    }
    virtual size_t get_id()
    {
        return -1;
    }
    virtual Class_id get_base_cid()
    {
        return c_id;
    }

    virtual bool check_rect(unsigned int px, unsigned int py, int )
    {
        return (px == location.get_world_x() && py == location.get_world_y());
    }

    virtual char * get_description()
    {
        char * buf = new char[128];
        sprintf(buf, "%s: (%s)", get_class_name(), get_name());
        return buf;
    }
    virtual Property ** get_properties(int * count)
    {
        *count = 0;
        return nullptr;
    }
    virtual ~InventoryElement()
    {
    }
    virtual void update_item_location(ItemLocation &, ItemLocation &)
    {
    }
    virtual Form get_form()
    {
        return Form_unknown;
    }
    virtual const char * get_form_name()
    {
        return Form_name[Form_unknown];
    }
    virtual bool can_pickup()
    {
        return false;
    }
};

enum Place_id
{
    PLACE_FIELD,
    PLACES_COUNT,
};

extern const char * places_names[];

enum Place_states
{    
    FIELD_PLOWED,
    FIELD_PLANTED,
};

extern const char * place_states_names[];

class Place : public InventoryElement
{
  public:
    Place_id id;
    Place_states state;

    const char * get_name() override
    {
        return places_names[id];
    }
    Place(Place_id id, size_t uid);
    Place(Place_id id);
    void show(bool details = true) override
    {
        InventoryElement::show(details);
        CONSOLE_LOG("Place type: %s state: %s\n", get_name(), place_states_names[state]);
    }
    char * get_description() override
    {
        char * buf = new char[128];
        sprintf(buf, "%s: (%s)", get_name(), place_states_names[state]);
        return buf;
    }
    virtual void show_state() {}
    size_t get_id() override
    {
        return (size_t) id;
    }
};

class Scroll : public InventoryElement
{
    SerializablePointer<Base> base;

  public:
    Base * get_base()
    {
        return base.get();
    }
    Scroll(Base * b);
    Class_id get_base_cid() override
    {
        return get_base()->c_id;
    }
    char * get_description() override
    {
        char * buf = new char[128];
        sprintf(buf, "%s describing %s", get_class_name(), get_name());
        return buf;
    }
    const char * get_name() override
    {
        return get_base()->get_name();
    }
    size_t get_id() override
    {
        return get_base()->uid;
    }
    void show(bool details) override
    {
        InventoryElement::show(details);
        if (details)
            get_base()->show(details);
    }
};

class Element : public InventoryElement
{
    SerializablePointer<BaseElement> base;
    // void init(BaseElement * b);
  protected:
    Property length;
    Property width;
    Property height;
    Property volume;     // lenght*width*height
    Property sharpness;  // ostrość
    Property smoothness; // gładkość
    Property mass;       // density*volume

  public:
    BaseElement * get_base()
    {
        return base.get();
    }
    void show(bool details = true) override;
    Element(BaseElement * b);
#ifdef USE_ENET
    Element(int id);
#endif
    Form get_form() override
    {
        return get_base()->form;
    }
    const char * get_name() override
    {
        return get_base()->get_name();
    }
    const char * get_form_name() override
    {
        return Form_name[get_form()];
    }
    size_t get_id() override
    {
        return get_base()->uid;
    }
    Class_id get_base_cid() override
    {
        return get_base()->c_id;
    }

    Property ** get_properties(int * count) override
    {
        *count = 8;
        Form f = get_form();
        if (f == Form_solid)
            *count += 6;
        Property ** props = new Property *[*count];
        props[0] = &length;
        props[1] = &width;
        props[2] = &height;
        props[3] = &volume;
        props[4] = &sharpness;
        props[5] = &smoothness;
        props[6] = &mass;
        props[7] = &get_base()->density;
        if (f == Form_solid)
        {
            props[8] = &get_base()->solid.tooling;
            props[9] = &get_base()->solid.stretching;
            props[10] = &get_base()->solid.squeezing;
            props[11] = &get_base()->solid.bending;
            props[12] = &get_base()->solid.solubility;
            props[13] = &get_base()->solid.hardness;
        }
        return props;
    }
    char * get_description() override
    {
        char * buf = new char[128];
        sprintf(buf, "%s %s: (%s) base=%d", get_form_name(), get_class_name(), get_name(), get_id());
        return buf;
    }

};

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
    PROD_COUNT
};

extern const char * ingredient_name[];
extern const char * product_name[];

class Ingredient : public InventoryElement
{
    size_t padding; // FIXME

  public:
    // properties only needed to create product
    Property quality;    //[0..100] slaby..najlepszy
    Property resilience; // [0..100] wytrzymały..słaby
    Property usage;      // [0..100] łatwy..trudny

    Ingredient_id id;
    Form req_form;

    Property ** get_properties(int * count)
    {
        Property ** props = new Property *[3];
        props[0] = &quality;
        props[1] = &resilience;
        props[2] = &usage;

        *count = 3;
        return props;
    }
    size_t get_id()
    {
        return id;
    }
    Ingredient(Ingredient_id i);

    void show(bool details = true);

    const char * get_name()
    {
        return ingredient_name[id];
    }
    char * get_description()
    {
        char * buf = new char[128];
        sprintf(buf, "%s: (%s)", get_class_name(), ingredient_name[id]);
        return buf;
    }
    bool action(Product_action action, Player * pl)
    {
        CONSOLE_LOG("ING: %s %s\n", product_action_name[action], get_name());
        return false;
    }
};

class Product : public InventoryElement
{
    size_t padding; // FIXME
  public:
    Property quality;    //[0..100] slaby..najlepszy
    Property resilience; // [0..100] wytrzymały..słaby
    Property usage;      // [0..100] łatwy..trudny
    Form req_form;

    Product_action actions[10];
    int actions_count;

    Product_id id;
    size_t get_id() override
    {
        return id;
    }
    Product(Product_id id, int actions_count);
    virtual void add_action(Product_action * a)
    {
    }
    Property ** get_properties(int * count) override
    {
        Property ** props = new Property *[3];
        props[0] = &quality;
        props[1] = &resilience;
        props[2] = &usage;

        *count = 3;
        return props;
    }
    virtual bool check_ing()
    {
        return false;
    }
    void show(bool details = true) override;

    const char * get_name() override
    {
        return product_name[id];
    }
    char * get_description() override
    {
        char * buf = new char[128];
        sprintf(buf, "%s: (%s)", get_class_name(), product_name[id]);
        return buf;
    }
};

class BaseAnimal : public Base
{
  public:
    bool carnivorous; // mięsożerny
    bool swimming;
    bool flying;

    BaseAnimal(int index);
    void show(bool details = true)
    {
        CONSOLE_LOG("BaseAnimal:\n");
        CONSOLE_LOG("carnivorous=%d\n", carnivorous);
        CONSOLE_LOG("swimming=%d\n", swimming);
        CONSOLE_LOG("flying=%d\n", flying);
        if (details)
            Base::show(details);
    }
    virtual size_t get_size()
    {
        return sizeof(BaseAnimal);
    }
};

class Animal : public InventoryElement
{
    // int padding; // FIXME
    SerializablePointer<BaseAnimal> base;
    void init(BaseAnimal * b);

  public:
    float size;
    Animal(BaseAnimal * b);
#ifdef USE_ENET
    Animal(int id);
#endif
    void show(bool details = true) override
    {
        InventoryElement::show(details);

        if (details)
            get_base()->show(details);
    }
    /* bool tick()
     {
         return grow();
     }*/

    BaseAnimal * get_base()
    {
        return base.get();
    }
    size_t get_id() override
    {
        return get_base()->uid;
    }

    Class_id get_base_cid() override
    {
        return get_base()->c_id;
    }

    bool action(Product_action action, Player * pl) override
    {
        CONSOLE_LOG("ANIMAL: %s %s\n", product_action_name[action], get_name());
        return false;
    }
    const char * get_name() override
    {
        return get_base()->get_name();
    }
    char * get_description() override
    {
        char * buf = new char[128];
        sprintf(buf, "%s: (%s)", get_class_name(), get_name());
        return buf;
    }
};

enum Plant_phase
{    
    Plant_seedling,
    Plant_growing,
    Plant_flowers,
    Plant_fruits
};

extern const char * plant_phase_name[];

class BasePlant : public Base
{
  public:
    bool flowers;
    bool leaves;
    BasePlant(int index);
    void show(bool details = true)
    {
        CONSOLE_LOG("BasePlant:\n");
        CONSOLE_LOG("flowers=%d\n", flowers);
        CONSOLE_LOG("leaves=%d\n", leaves);
        if (details)
            Base::show(details);
    }
    virtual size_t get_size()
    {
        return sizeof(BasePlant);
    }
};

class Plant : public InventoryElement
{
    //   int padding; // FIXME
    SerializablePointer<BasePlant> base;
    void init(BasePlant * b);

  protected:
    unsigned int seedling_time;
    unsigned int growing_time;
    unsigned int flowers_time;

  public:
    float size;    
    int water;

    // shared with client

    Plant_phase phase;
    bool grown;

    Plant(BasePlant * b);
#ifdef USE_ENET
    Plant(int id);
#endif
    void show(bool details = true) override
    {
        InventoryElement::show(details);
        CONSOLE_LOG("%s\n", get_base()->get_name());
        if (details)
        {
            get_base()->show(details);
            CONSOLE_LOG("phase=%s grown=%d times=%d/%d/%d/ water=%d \n", plant_phase_name[phase], grown,  seedling_time, growing_time, flowers_time, water);
        }
    }
    BasePlant * get_base()
    {
        return base.get();
    }
    const char * get_name()
    {
        return get_base()->get_name();
    }
    size_t get_id() override
    {
        return get_base()->uid;
    }
    Class_id get_base_cid() override
    {
        return get_base()->c_id;
    }
    bool action(Product_action action, Player * pl)
    {
        CONSOLE_LOG("PLANT: %s %s\n", product_action_name[action], get_name());
        return false;
    }

    char * get_description() override
    {
        char * buf = new char[128];
        sprintf(buf, "%s: (%s) %s", get_class_name(), get_name(), plant_phase_name[phase]);
        return buf;
    }
    Property ** get_properties(int * count)
    {
        Property ** props = new Property *[1];
        props[0] = new Property(plant_phase_name[phase], phase);

        *count = 1;
        return props;
    }
};

#endif
