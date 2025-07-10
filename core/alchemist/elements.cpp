#include <cstdio>
#include <cstdlib>
#include <cstring>

#include "../tiles.h"
#include "elements.h"
#include "names.h"

ElementsList * base_list;

const char * Ingredient_name[] = {
    "Axe blade",
    "Axe handle",

    "Knife blade",
    "Knife handle",

    "Pickaxe blade",
    "Pickaxe handle",

    "Wall",

};

const char * Product_name[] = {
    "Axe",
    "Knife",
    "Pickaxe",
    "Hut",
};

const char * object_names[] = {"wall"};
const char * Plant_phase_name[] = {"Seed", "Seedling", "Growing", "Flowers", "Fruits"};
const char * Class_names[] = {"unknown", "BaseElement", "BaseAnimal", "BasePlant", "Element", "Ingredient", "Product", "Being", "Plant", "Animal", "Player", "Npc"};


BaseElement::BaseElement(Form f, int index) : Base(index, Class_BaseElement)
{
    solid = NULL;
    edible = NULL;
    
    color.r = rand() % 256;
    color.g = rand() % 256;
    color.b = rand() % 256;
    
    density = nullptr;
    form = f;
    switch (f)
    {
        case Form_solid:
            solid = new Solid;
            density = new Property("density", 50 + rand() % 2000);
            break;
        case Form_liquid:
            density = new Property("density", 500 + rand() % 500);
            break;
        case Form_gas:
            density = new Property("density", 1);
            break;
    }
    if (rand() % 100 < 10) // 10% edible
        edible = new Edible;

    name = create_name(5 - form);
}

void BaseElement::show(bool details)
{
    Base::show(details);
    printf("BaseElement form=%s edible=%s\n", Form_name[form], edible ? "yes" : "no");
    if (!details)
        return;
    density->show(); // gęstość
    printf("   form = %s\n", Form_name[form]);
    switch (form)
    {
        case Form_solid:
            solid->show();
            break;
        default:
            break;
    }
    if (edible)
        edible->show();
}

Element::Element()
{
    Class_id id = Class_BaseElement;
    BaseTable * table = dynamic_cast<BaseTable *>(base_list->find(&id));
    init(dynamic_cast<BaseElement *>(table->get_random()));
}
Element::Element(BaseElement * b)
{
    init(b);
}

Element::Element(int i)
{
    Class_id id = Class_BaseElement;
    BaseTable * table = dynamic_cast<BaseTable *>(base_list->find(&id));
    init(dynamic_cast<BaseElement *>(table->get(i)));
}

void Element::init(BaseElement * b)
{
    c_id = Class_Element;
    base = b;
    sharpness = new Property("sharpeness", rand() % 100);
    smoothness = new Property("smoothness", rand() % 100);
    length = new Property("length", 1 + rand() % 100);
    width = new Property("width", 1 + rand() % 100);
    height = new Property("height", 1 + rand() % 100);
    volume = new Property("volume", length->value * width->value * height->value);
    mass = new Property("mass", b->density->value * volume->value / 1000);
}

void Element::show(bool details)
{
    printf("%s: base=%s form=%s uid=%lx\n", Class_names[c_id], base->name, get_form_name(), uid);
    if (!details)
        return;
    sharpness->show();  // ostrość
    smoothness->show(); // gładkość
    length->show();
    width->show();
    height->show();
    volume->show();
    mass->show();

    base->show(details);
}

#ifdef CORE_FOR_CLIENT
Ingredient::Ingredient(Ingredient_id i)
{
    c_id = Class_Ingredient;
    name = Ingredient_name[i];
    id = i;

    quality = new Property("quality", 0);
    resilience = new Property("resilience", 0);
    usage = new Property("usage", 0);
    crafted = true;
}
#else
Ingredient::Ingredient(InventoryElement * from, Ingredient_id i, Form f)
{
    c_id = Class_Ingredient;
    el = from;
    name = Ingredient_name[i];
    id = i;
    req_form = f;
    quality = nullptr;
    resilience = nullptr;
    usage = nullptr;
}
bool Ingredient::craft()
{
    if (req_form != el->get_form())
    {
        printf("form != %d\n", req_form);
        return false;
    }

    quality = new Property("quality", rand() % 100);
    resilience = new Property("resilience", rand() % 100);
    usage = new Property("usage", rand() % 100);
    crafted = true;
    return true;
}
#endif

void Ingredient::show(bool details)
{
    printf("%s ->%d\n", name, c_id);
    if (!details)
        return;
    quality->show();
    resilience->show();
    usage->show();
    printf("form = %s", Form_name[req_form]);
}

#ifdef CORE_FOR_CLIENT
Product::Product(Product_id i)
{
    id = i;
    c_id = Class_Product;
    name = Product_name[i];
    quality = new Property("quality", 0);
    resilience = new Property("resilience", 0);
    usage = new Property("usage", 0);
    crafted = true;
}
#else
void Product::init(Product_id i, int c, Form f)
{
    name = Product_name[i];
    id = i;
    ing_count = c;
    req_form = f;
}

Product::Product(InventoryElement * el1, InventoryElement * el2, Product_id i, Form f)
{
    c_id = Class_Product;
    ings = (InventoryElement **)calloc(2, sizeof(InventoryElement));
    ings[0] = el1;
    ings[1] = el2;
    init(i, 2, f);
}

Product::Product(InventoryElement ** from, int count, Product_id i, Form f)
{
    c_id = Class_Product;
    ings = from;
    init(i, count, f);
}

bool Product::craft() // executed only on server
{
    for (int i = 0; i < ing_count; i++)
    {
        if (req_form != ings[i]->get_form())
        {
            printf("form != %d for inq[%d]\n", req_form, i);
            return false;
        }
    }
    if (!check_ing())
        return false;

    quality = new Property("quality", rand() % 100);
    resilience = new Property("resilience", rand() % 100);
    usage = new Property("usage", rand() % 100);
    crafted = true;
    return true;
}
#endif

void Product::show(bool details)
{
    printf("%s -> %d\n", name, c_id);
    if (!details)
        return;
    quality->show();
    resilience->show();
    usage->show();
}

void init_elements()
{
    base_list = new ElementsList("base list");

    BaseTable * base_elements = new BaseTable(BASE_ELEMENTS, Class_BaseElement);

    int i = 0;

    for (; i < SOLID_ELEMENTS; i++)
    {
        base_elements->add(i, new BaseElement(Form_solid, i));
    }
    for (; i < SOLID_ELEMENTS + LIQUID_ELEMENTS; i++)
    {
        base_elements->add(i, new BaseElement(Form_liquid, i));
    }
    for (; i < SOLID_ELEMENTS + LIQUID_ELEMENTS + GAS_ELEMENTS; i++)
    {
        base_elements->add(i, new BaseElement(Form_gas, i));
    }
    base_list->add(base_elements);

    BaseTable * base_animals = new BaseTable(BASE_ANIMALS, Class_BaseAnimal);

    for (i = 0; i < BASE_ANIMALS; i++)
    {
        base_animals->add(i, new BaseAnimal(i));
    }
    base_list->add(base_animals);

    BaseTable * base_plants = new BaseTable(BASE_PLANTS, Class_BasePlant);

    for (i = 0; i < BASE_PLANTS; i++)
    {
        base_plants->add(i, new BasePlant(i));
    }
    base_list->add(base_plants);
}

void show_base_table(Class_id id, bool details)
{
    BaseTable * table = dynamic_cast<BaseTable *>(base_list->find(&id));
    table->show(details);
}

void Animal::init(BaseAnimal * b)
{
    c_id = Class_Animal;
    base = b;
    alive = true;
    max_age = new Property("max age", 1 + rand() % 36000); // 100 years
    age = new Property("age", rand() % max_age->value);
    name = create_name(rand() % 2 + 2);
    can_talk = false;
}

BaseAnimal::BaseAnimal(int index) : Base(index, Class_BaseAnimal)
{
    id = index;
    carnivorous = rand() % 2;
    swimming = rand() % 2;
    flying = rand() % 2;
    name = create_name(10);
}

Animal::Animal(BaseAnimal * b)
{
    init(b);
}

Animal::Animal()
{
    Class_id id = Class_BaseAnimal;
    BaseTable * table = dynamic_cast<BaseTable *>(base_list->find(&id));
    init(dynamic_cast<BaseAnimal *>(table->get_random()));
}

Animal::Animal(int i)
{
    Class_id id = Class_BaseAnimal;
    BaseTable * table = dynamic_cast<BaseTable *>(base_list->find(&id));
    init(dynamic_cast<BaseAnimal *>(table->get(i)));
}

BasePlant::BasePlant(int index) : Base(index, Class_BasePlant)
{
    flowers = rand() % 2;
    leaves = rand() % 2;
    name = create_name(15);
}

Plant::Plant()
{
    Class_id id = Class_BasePlant;
    BaseTable * table = dynamic_cast<BaseTable *>(base_list->find(&id));
    init(dynamic_cast<BasePlant *>(table->get_random()));
}

Plant::Plant(int i)
{
    Class_id id = Class_BasePlant;
    BaseTable * table = dynamic_cast<BaseTable *>(base_list->find(&id));
    init(dynamic_cast<BasePlant *>(table->get(i)));
}

// FIXME split Plant to sertver/client side
void Plant::init(BasePlant * b)
{
    c_id = Class_Plant;
    base = b;
    seedling_time = 7 + rand() % 14;
    growing_time = seedling_time + rand() % 150;
    flowers_time = growing_time + rand() % 30;
    max_age = new Property("max age", flowers_time + rand() % 100);
    phase = (Plant_phase)(rand() % (Plant_fruits + 1));
    grown = false;
    water = rand() % 100;
    age = new Property("age", 0);
    can_talk = false;
    switch (phase)
    {
        case Plant_seed:
            age->value = 1;
            planted = false;
            break;
        case Plant_seedling:
            age->value = seedling_time;
            planted = true;
            break;
        case Plant_growing:
            age->value = growing_time;
            planted = true;
            break;
        case Plant_flowers:
            age->value = flowers_time;
            planted = true;
            break;
        case Plant_fruits:
            age->value = max_age->value;
            grown = true;
            planted = true;
            break;
    }
    name = create_name(rand() % 3 + 5);
}

Plant::Plant(BasePlant * b)
{
    init(b);
}

BaseElement* get_base_element(int id)
{
    BaseTable* table = (BaseTable*)base_list->head;
    return (BaseElement*)table->get(id);
}
