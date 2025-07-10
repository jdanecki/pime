#ifndef __ELEMENTS__H
#define __ELEMENTS__H

#include "names.h"
#include <cstdio>
#include <cstdlib>
#include <stdint.h>
#include "properties.h"

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
    const char * name;
    int id; // index in BaseTable
    Class_id c_id;
    Base(int index, Class_id c)
    {
        id = index;
        name = nullptr;
        c_id = c;
    }
    virtual void show(bool details = true)
    {
        printf("Base name=%s id=%d \n", name, id);
    }
};

class BaseElement : public Base
{
  public:
    Property * density;
    Edible * edible; // not known by default, it should depend on player/animal/npc
    Form form; //solid, liquid, gas
    Solid * solid;
    struct {int r; int g; int b;} color;

    BaseElement(Form f, int index);
    ~BaseElement()
    {
        delete density;
    }
    void show(bool details = true);
};

enum ItemLocationType
{
    LOCATION_CHUNK,
    LOCATION_PLAYER_INV,
};

union ItemLocationData
{
    struct
    {
        int map_x, map_y, x, y;
    } chunk;
    struct
    {
        int id;
    } player;
};

class ItemLocation
{
  public:
    enum ItemLocationType type;
    ItemLocationData data;
};

class chunk;

class InventoryElement
{
    // int x, y, z;
  protected:
    const char * name;

  public:
    bool crafted;
    bool pickable;
    ItemLocation location;
    size_t uid;
    Class_id c_id;
    Form req_form;

    InventoryElement()
    {
        req_form = Form_unknown;
        uid = (size_t)this;
        name = nullptr;
        crafted = false;
        pickable=true;
    }
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
    virtual Form get_form()
    {
        return req_form;
    }
    virtual const char * get_name()
    {
        return name;
    }
    const char * get_class_name()
    {
        return Class_names[c_id];
    }
    virtual const char * get_form_name()
    {
        return Form_name[req_form];
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
        return location.data.chunk.x;
    }
    int get_y()
    {
        return location.data.chunk.y;
    }
    size_t get_uid()
    {
        return uid;
    }
    virtual char * get_description()
    {
        char * buf = new char[128];
        sprintf(buf, "%s: %s (%s)", get_class_name(), get_form_name(), get_name());
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

class Element : public InventoryElement
{
    BaseElement * base;
    void init(BaseElement * b);

  public:
    Property * sharpness;
    Property * smoothness;
    Property * mass; // density*volume
    Property * length;
    Property * width;
    Property * height;
    Property * volume; // lenght*width*height

    Property ** get_properties(int * count)
    {
        Property ** props = new Property *[7];
        props[0] = sharpness;
        props[1] = smoothness;
        props[2] = mass;
        props[3] = length;
        props[4] = width;
        props[5] = height;
        props[6] = volume;
        *count = 7;
        return props;
    }
    BaseElement * get_base()
    {
        return base;
    }
    Edible * get_edible()
    {
        return base->edible;
    }

    void show(bool details = true);
    Element();
    Element(BaseElement * b);
    Element(int i);
    ~Element()
    {
        delete sharpness;
        delete smoothness;
        delete mass;
        delete length;
        delete width;
        delete height;
        delete volume;
    }
    Form get_form()
    {
        return base->form;
    }
    const char * get_name()
    {
        return base->name;
    }
    const char * get_form_name()
    {
        return Form_name[base->form];
    }
    int get_id() override
    {
        return base->id;
    }
    Class_id get_base_cid()
    {
        return base->c_id;
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
    int * padding; // FIXME

  public:
    Property * quality;    //[0..100] slaby..najlepszy
    Property * resilience; // [0..100] wytrzymały..słaby
    Property * usage;      // [0..100] łatwy..trudny

    Ingredient_id id;

    int get_id()
    {
        return id;
    }
#ifdef CORE_FOR_CLIENT
    Ingredient(Ingredient_id i);
    Property ** get_properties(int * count)
    {
        Property ** props = new Property *[3];
        props[0] = quality;
        props[1] = resilience;
        props[2] = usage;

        *count = 3;
        return props;
    }

#else
    InventoryElement * el; // available only in server , move to IngredientServer class
    bool craft();
    Ingredient(InventoryElement * from, Ingredient_id i, Form f);
#endif
    Edible * get_edible()
    { // FIXME
        return nullptr;
        // return el->get_edible();
    }

    ~Ingredient()
    {
        delete quality;
        delete resilience;
        delete usage;
    }
    void show(bool details = true);

    char * get_description()
    {
        char * buf = new char[128];
        sprintf(buf, "%s: (%s)", get_class_name(), get_name());
        return buf;
    }
};

class Product : public InventoryElement
{
    int * padding; // FIXME
#ifndef CORE_FOR_CLIENT
    void init(Product_id i, int c, Form f);
#endif
  public:
    Property * quality;    //[0..100] slaby..najlepszy
    Property * resilience; // [0..100] wytrzymały..słaby
    Property * usage;      // [0..100] łatwy..trudny

    Product_id id;
    int get_id() override
    {
        return id;
    }
#ifdef CORE_FOR_CLIENT
    Product(Product_id i);
    Property ** get_properties(int * count)
    {
        Property ** props = new Property *[3];
        props[0] = quality;
        props[1] = resilience;
        props[2] = usage;

        *count = 3;
        return props;
    }
#else
    int ing_count;
    InventoryElement ** ings;
    bool craft() override;

    Product(InventoryElement * el1, InventoryElement * el2, Product_id i, Form f);
    Product(InventoryElement ** from, int count, Product_id i, Form f);
#endif
    ~Product()
    {
        delete quality;
        delete resilience;
        delete usage;
    }
    virtual bool check_ing()
    {
        return false;
    }
    void show(bool details = true) override;

    char * get_description() override
    {
        char * buf = new char[128];
        sprintf(buf, "%s: (%s)", get_class_name(), get_name());
        return buf;
    }
    bool use(InventoryElement * object) override
    {
        printf("%s: use %s\n", get_name(), object->get_name());
        return object->use(this);
    }
};

class Being : public InventoryElement
{
    int * padding; // FIXME

  public:
    // shared with client
    Property * age;
    Property * max_age;

    bool alive;
    bool can_talk;

    virtual bool grow()
    {
        if (!alive)
        {
            //  printf("%s is dead\n", get_name());
            return false;
        }
        age->value++;
        // printf("%s:%s growing\n", get_class_name(), get_name());
        if (age->value >= max_age->value)
        {
            alive = false;
            printf("%s is dying\n", get_name());
        }
        return alive;
    }
    Being()
    {
        alive = true;
        c_id = Class_Being;
        max_age = new Property("max age", 1 + rand() % 36000); // 100 years
        age = new Property("age", rand() % max_age->value);

        name = nullptr;
        req_form = Form_solid;
        can_talk = false;
    }
    Property ** get_properties(int * count)
    {
        Property ** props = new Property *[2];
        props[0] = age;
        props[1] = max_age;

        *count = 2;
        return props;
    }
    ~Being()
    {
        delete age;
        delete max_age;
    }
    bool is_alive()
    {
        return alive;
    }
    void show(bool details = true)
    {
        printf("%s %s alive=%d uid=%lx\n", Class_names[c_id], name, alive, uid);
        age->show();
        max_age->show();
    }
    bool tick()
    {
        return grow();
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

class Animal : public Being
{
    // int padding; // FIXME
    BaseAnimal * base;
    void init(BaseAnimal * b);

  public:
    // shared with client

    Animal(BaseAnimal * b);
    Animal();
    Animal(int i);
    void show(bool details = true) override
    {
        printf("Animal %s alive=%d uid=%lx\n", name, alive, uid);
        age->show();
        max_age->show();
        if (details)
            base->show(details);
    }
    /* bool tick()
     {
         return grow();
     }*/
    int get_id() override
    {
        return base->id;
    }

    Class_id get_base_cid() override
    {
        return base->c_id;
    }
    bool use(InventoryElement * object) override
    {
        printf("using %s on %s, do you want to kill it?\n", object->get_name(), get_name());
        return false;
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

class Plant : public Being
{
    //   int padding; // FIXME
    BasePlant * base;
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
    Plant();
    Plant(int i);
    void show(bool details = true) override
    {
        printf("Plant -> %d name=%s grown=%d uid=%lx\n", c_id, name, grown, uid);
        age->show();
        max_age->show();
        if (details)
        {
            printf("phase=%s planted=%d times=%d/%d/%d/%d water=%d\n", Plant_phase_name[phase], planted, seedling_time, growing_time, flowers_time, max_age->value, water);
        }
    }
    void sow()
    {
        planted = 1;
        change_phase(Plant_seedling);
    }
    void change_phase(Plant_phase p)
    {
        if (phase != p)
        {
            switch (phase)
            {
                case Plant_seedling:
                    age->value = 1;
                    break;
                case Plant_seed:
                    age->value = 0;
                    break;
            }
            printf("%s changing phase: %s -> %s age=%u/%u\n", name, Plant_phase_name[phase], Plant_phase_name[p], age->value, max_age->value);
        }
        phase = p;
    }
    int get_id() override
    {
        return base->id;
    }
    Class_id get_base_cid() override
    {
        return base->c_id;
    }
    bool use(InventoryElement * object) override
    {
        printf("using %s on %s, are you sure?\n", object->get_name(), get_name());
        return false;
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

BaseElement* get_base_element(int id);

#endif
