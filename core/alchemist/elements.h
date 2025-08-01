#ifndef __ELEMENTS__H
#define __ELEMENTS__H

#include "names.h"
#include <cstddef>
#include <cstdio>
#include <cstdlib>
#include <stdint.h>
#include "properties.h"
#include "item_location.h"

enum Class_id
{
    Class_Unknown = 0,
    Class_BaseElement,
    Class_BaseAnimal,
    Class_BasePlant,

    Class_Element,
    Class_Ingredient,
    Class_Product,

    Class_Plant,
    Class_Animal,
    Class_Player,
    Class_Npc,
};

extern const char * class_name[];

enum Product_action
{
    ACT_NOTHING,
    ACT_CUT,
    ACT_HIT,
    ACT_STAB, // dźgnij
    ACT_FIRE,
};

extern const char * product_action_name[];

enum Player_action
{
    PLAYER_DRINK,
    PLAYER_EAT
};

extern const char * player_action_name[];

enum Server_action
{
    SERVER_SHOW_ITEM,
};

extern const char * server_action_name[];

class Base
{
  public:
    Class_id c_id;
    int id;
    SerializableCString name;
    Base(int index, Class_id c, const char * name);
    virtual void show(bool details = true);
    const char * get_name();
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
    Color color;

    BaseElement(Form f, Color color, int index);
};

class chunk;
class Player;

class InventoryElement
{
    // int x, y, z;
  public:
    Class_id c_id;
    size_t uid;

    ItemLocation location;

    InventoryElement(Class_id c_id, size_t uid, ItemLocation location);
    InventoryElement(Class_id c_id)
    {
        this->c_id = c_id;
        uid = (size_t)this;
    }

    virtual bool action(Product_action action, Player * pl)
    {
        printf("INV: %s %s\n", product_action_name[action], get_name());
        return false;
    }
    virtual bool player_action(Player_action action, Player * pl)
    {
        printf("INV: %s %s\n", player_action_name[action], get_name());
        return false;
    }
    virtual void show(bool details = true)
    {
        printf("%s\n", get_class_name());
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
    virtual int get_id()
    {
        return -1;
    }
    virtual Class_id get_base_cid()
    {
        return c_id;
    }
    virtual Class_id get_cid()
    {
        return c_id;
    }
    virtual Edible * get_edible()
    {
        return nullptr;
    }

#ifndef CORE_FOR_CLIENT

    virtual bool craft()
    {
        printf("missing craft function\n");
        return false;
    }
#endif
    int get_x()
    {
        return location.chunk.x;
    }
    int get_y()
    {
        return location.chunk.y;
    }
    size_t get_uid()
    {
        return uid;
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

enum object_types
{
    OBJECT_wall,
};

extern const char * object_names[];

#define OBJECTS 1

class Object : public InventoryElement
{
  public:
    BaseElement * base;
    enum object_types type;

    Form get_form()
    {
        return Form_solid;
    }
    const char * get_form_name()
    {
        return Form_name[Form_solid];
    }
    const char * get_name()
    {
        return object_names[type];
    }
    void show(bool details = true) override
    {
        printf("Object type: %s", get_name());
        base->show(details);
    }
};

template <typename T> class SerializablePointer
{
    T * ptr;

  public:
    SerializablePointer(T * p);
    T * get()
    {
        return ptr;
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
    Property volume; // lenght*width*height

  public:
    BaseElement * get_base()
    {
        return base.get();
    }
    void show(bool details = true) override;
    Element(BaseElement * b);
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
        return Form_name[get_base()->form];
    }
    int get_id() override
    {
        return get_base()->id;
    }
    Class_id get_base_cid() override
    {
        return get_base()->c_id;
    }
    bool action(Product_action action, Player * pl) override
    {
        printf("ELEMENT: %s %s\n", product_action_name[action], get_name());
        return false;
    }
    char * get_description() override
    {
        char * buf = new char[128];
        sprintf(buf, "%s: (%s) ", get_class_name(), get_name());
        return buf;
    }
    Property ** get_properties(int * count) override
    {
        // FIXME
        Property ** props = new Property *[4];
        props[0] = &length;
        props[1] = &width;
        props[2] = &height;
        props[3] = &volume;
        *count = 4;
        return props;
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

    ING_WALL,

    ING_MEAT,

    ING_LOG,
    ING_TINDER,
    ING_STICK,

    ING_FRUIT,

    ING_COUNT,

};

enum Product_id
{
    PROD_AXE,
    PROD_KNIFE,
    PROD_PICKAXE,
    PROD_HUT,
    PROD_FIRE,
    PROD_ROASTED_MEAT,
    PROD_FRUIT_SALAD,

    PROD_COUNT
};

extern const char * ingredient_name[];
extern const char * product_name[];
extern const char * items_name[];

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
    int get_id()
    {
        return id;
    }
    Ingredient(Ingredient_id i);

    Edible * get_edible()
    { // FIXME
        return nullptr;
        // return el->get_edible();
    }

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
    virtual bool check_ing()
    {
        return false;
    }
    bool action(Product_action action, Player * pl)
    {
        printf("ING: %s %s\n", product_action_name[action], get_name());
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

    // FIMXE change it to Product_action *
    Product_action actions;
    int actions_count;

    Product_id id;
    int get_id() override
    {
        return id;
    }
    Product(Product_id i);
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
        printf("BaseAnimal:\n");
        printf("carnivorous=%d\n", carnivorous);
        printf("swimming=%d\n", swimming);
        printf("flying=%d\n", flying);
        if (details)
            Base::show(details);
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

    void show(bool details = true) override
    {
        printf("Animal %s uid=%lx\n", get_name(), uid);
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
    int get_id() override
    {
        return get_base()->id;
    }

    Class_id get_base_cid() override
    {
        return get_base()->c_id;
    }

    bool action(Product_action action, Player * pl) override
    {
        printf("ANIMAL: %s %s\n", product_action_name[action], get_name());
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
    Plant_seed = 0,
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
        printf("BasePlant:\n");
        printf("flowers=%d\n", flowers);
        printf("leaves=%d\n", leaves);
        if (details)
            Base::show(details);
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
    bool planted;
    int water;

    // shared with client

    Plant_phase phase;
    bool grown;

    Plant(BasePlant * b);

    void show(bool details = true) override
    {
        InventoryElement::show(details);
        if (details)
        {
            get_base()->show(details);
            printf("phase=%s grown=%d planted=%d times=%d/%d/%d/ water=%d \n", plant_phase_name[phase], grown, planted, seedling_time, growing_time, flowers_time, water);
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
    int get_id() override
    {
        return get_base()->id;
    }
    Class_id get_base_cid() override
    {
        return get_base()->c_id;
    }
    bool action(Product_action action, Player * pl)
    {
        printf("PLANT: %s %s\n", product_action_name[action], get_name());
        return false;
    }

    char * get_description() override
    {
        char * buf = new char[128];
        sprintf(buf, "%s: (%s) %s", get_class_name(), get_name(), plant_phase_name[phase]);
        return buf;
    }
};

#define ING_ELEMENTS 11
#define PROD_ELEMENTS 6

#define BASE_ANIMALS 40
#define BASE_PLANTS 10

#endif
