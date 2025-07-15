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

    Class_Being,
    Class_Plant,
    Class_Animal,
    Class_Player,
    Class_Npc,
};

extern const char * Class_names[];

class Base
{
  public:
    Class_id c_id;
    int id; // index in BaseTable
    SerializableCString name;
    Base(int index, Class_id c, const char* name);
    virtual void show(bool details = true);
    const char* get_name();
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
    Form form; //solid, liquid, gas
    Color color;

    BaseElement(Form f, Color color, int index);
};

class chunk;

class InventoryElement
{
    // int x, y, z;
  protected:
    const char * name;

  public:
    // Property mass; // density*volume // FIXME maybe
    unsigned int mass;
    ItemLocation location;
    size_t uid;
    Class_id c_id;

    InventoryElement(); //: mass("unknown element", 0)
    InventoryElement(Class_id c_id, size_t uid, unsigned int mass, ItemLocation location);
    virtual bool use(int map_x, int map_y, int x, int y)
    {
        return false;
    }
    virtual bool use(InventoryElement * object)
    {
        printf("I don't know how to use %s on %s\n", object->get_name(), get_name());
        return false;
    }
    virtual void show(bool details = true)
    {
    }
    virtual bool tick()
    {
        return false;
    }
    virtual const char * get_name()
    {
        return name;
    }
    const char * get_class_name()
    {
        return Class_names[c_id];
    }
    virtual int get_id()
    {
        return -1;
    }
    virtual Class_id get_base_cid()
    {
        return Class_Unknown;
    }
    virtual Edible * get_edible()
    {
        return nullptr;
    }
    virtual BaseElement * get_base()
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
    void show()
    {
        printf("Object type: %s", get_name());
        base->show();
    }
};

template<typename T>
class SerializablePointer
{
    T* ptr;
    
public:
    SerializablePointer(T* p);
    T* get()
    {
        return ptr;
    }
};

class Element : public InventoryElement
{
    SerializablePointer<BaseElement> base;
    // void init(BaseElement * b);

    public:
    BaseElement * get_base() override
    {
        return base.get();
    }
    // void show(bool details = true) override;
    Element(BaseElement * b);
    Form get_form()
    {
        return base.get()->form;
    }
    const char * get_name() override
    {
        return base.get()->get_name();
    }
    const char * get_form_name()
    {
        return Form_name[base.get()->form];
    }
    int get_id() override
    {
        return base.get()->id;
    }
    Class_id get_base_cid() override
    {
        return base.get()->c_id;
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

    ING_NUM,

};

enum Product_id
{
    PROD_AXE,
    PROD_KNIFE,
    PROD_PICKAXE,
    PROD_HUT,
};

extern const char * Ingredient_name[];
extern const char * Product_name[];
extern const char * items_name[];

class Ingredient : public InventoryElement
{
    size_t padding; // FIXME

  public:
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

    char * get_description()
    {
        char * buf = new char[128];
        sprintf(buf, "%s: (%s)", get_class_name(), Ingredient_name[id]);
        return buf;
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

    Product_id id;
    int get_id() override
    {
        return id;
    }
    Product(Product_id i);
    Property ** get_properties(int * count)
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

    char * get_description() override
    {
        char * buf = new char[128];
        sprintf(buf, "%s: (%s)", get_class_name(), Product_name[id]);
        return buf;
    }
    bool use(InventoryElement * object) override
    {
        printf("%s: use %s\n", get_name(), object->get_name());
        return object->use(this);
    }
};

// FIXME is it even needed?
// class Being : public InventoryElement
// {
//     size_t padding; // FIXME

//   public:
//     // shared with client

//     bool alive;
//     bool can_talk;

//     Being()
//     {
//         alive = true;
//         c_id = Class_Being;

//         name = nullptr;
//         can_talk = false;
//     }
//     bool is_alive()
//     {
//         return alive;
//     }
//     void show(bool details = true)
//     {
//         printf("%s %s alive=%d uid=%lx\n", Class_names[c_id], name, alive, uid);
//     }
// };

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
    Animal(BaseAnimal * b);
    // Animal();
    // Animal(int i);
    void show(bool details = true) override
    {
        printf("Animal %s uid=%lx\n", name, uid);
        if (details)
            base.get()->show(details);
    }
    /* bool tick()
     {
         return grow();
     }*/
    int get_id() override
    {
        return base.get()->id;
    }

    Class_id get_base_cid() override
    {
        return base.get()->c_id;
    }
    bool use(InventoryElement * object) override
    {
        printf("using %s on %s, do you want to kill it?\n", object->get_name(), get_name());
        return false;
    }
    char * get_description() override
    {
        char * buf = new char[128];
        sprintf(buf, "%s: (%s)", get_class_name(), base.get()->get_name());
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

extern const char * Plant_phase_name[];

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
    bool planted;
    int water;

    // shared with client

    Plant_phase phase;
    bool grown;

    Plant(BasePlant * b);
    // Plant();
    // Plant(int i);
    void show(bool details = true) override
    {
        printf("Plant -> %d name=%s grown=%d uid=%lx\n", c_id, name, grown, uid);
        if (details)
        {
            printf("phase=%s planted=%d times=%d/%d/%d/ water=%d\n", Plant_phase_name[phase], planted, seedling_time, growing_time, flowers_time, water);
        }
    }
    int get_id() override
    {
        return base.get()->id;
    }
    Class_id get_base_cid() override
    {
        return base.get()->c_id;
    }
    bool use(InventoryElement * object) override
    {
        printf("using %s on %s, are you sure?\n", object->get_name(), get_name());
        return false;
    }
    char * get_description() override
    {
        char * buf = new char[128];
        sprintf(buf, "%s: (%s)", get_class_name(), base.get()->get_name());
        return buf;
    }
};

#define SOLID_ELEMENTS 17
#define LIQUID_ELEMENTS 1
#define GAS_ELEMENTS 1
#define BASE_ELEMENTS (SOLID_ELEMENTS + LIQUID_ELEMENTS + GAS_ELEMENTS)
#define ING_ELEMENTS 7
#define PROD_ELEMENTS 4

#define BASE_ANIMALS 40
#define BASE_PLANTS 30

void init_elements();
void show_base_table(Class_id id, bool details);

// BaseElement* get_base_element(int id);

#endif
