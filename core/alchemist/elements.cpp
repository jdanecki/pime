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

const char * Product_action_names[] = {
    "nothing",
    "cut",
    "hit",
    "stab",
};

const char * object_names[] = {"wall"};
const char * Plant_phase_name[] = {"Seed", "Seedling", "Growing", "Flowers", "Fruits"};
const char * Class_names[] = {"unknown", "BaseElement", "BaseAnimal", "BasePlant", "Element", "Ingredient", "Product", "Plant", "Animal", "Player", "Npc"};

Base::Base(int index, Class_id c, const char * name) : name(name)
{
    id = index;
    c_id = c;
}

void Base::show(bool details)
{
    printf("Base name=%s class:%s id=%d \n", get_name(), Class_names[c_id], id);
}

const char * Base::get_name()
{
    return name.str;
}

BaseElement::BaseElement(Form f, Color color, int index) : Base(index, Class_BaseElement, create_name(5 - f)), form(f), color(color)
{
}

template <typename T> SerializablePointer<T>::SerializablePointer(T * p) : ptr(p)
{
}

Element::Element(BaseElement * b)
    : InventoryElement(Class_Element), base(b), length("length", rand() % 100), width("width", rand() % 100), height("height", rand() % 100),
      volume("volume", length.value * width.value * height.value)
{
}
InventoryElement::InventoryElement(Class_id c_id, size_t uid, unsigned int mass, ItemLocation location) : c_id(c_id), uid(uid), mass(mass), location(location)
{
}
void Element::show(bool details)
{
    printf("%s: base=%s form=%s uid=%lx\n", get_class_name(), get_name(), get_form_name(), uid);
    if (!details)
        return;
    length.show();
    width.show();
    height.show();
    volume.show();
    get_base()->show(details);
}

Ingredient::Ingredient(Ingredient_id i) : InventoryElement(Class_Ingredient), quality("quality", 0), resilience("resilience", 0), usage("usage", 0)
{
    id = i;
}

void Ingredient::show(bool details)
{
    printf("%s -> class:%s id=%d\n", get_name(), get_class_name(), id);
    if (!details)
        return;
    quality.show();
    resilience.show();
    usage.show();
    printf("form = %s", Form_name[req_form]);
}

Product::Product(Product_id i) : InventoryElement(Class_Product), quality("quality", 0), resilience("resilience", 0), usage("usage", 0)
{
    id = i;
    c_id = Class_Product;
    // actions = nullptr;
    actions = ACT_NOTHING;
    actions_count = 0;
}

void Product::show(bool details)
{
    printf("%s -> class:%s id=%d\n", get_name(), get_class_name(), id);
    if (!details)
        return;
    quality.show();
    resilience.show();
    usage.show();
}

void Animal::init(BaseAnimal * b)
{
    c_id = Class_Animal;
    // alive = true;
    // max_age = new Property("max age", 1 + rand() % 36000); // 100 years
    // age = new Property("age", rand() % max_age->value);
    // can_talk = false;
}

BaseAnimal::BaseAnimal(int index) : Base(index, Class_BaseAnimal, create_name(10))
{
    id = index;
    carnivorous = rand() % 2;
    swimming = rand() % 2;
    flying = rand() % 2;
}

Animal::Animal(BaseAnimal * b) : InventoryElement(Class_Animal), base(b)
{
    init(b);
}

BasePlant::BasePlant(int index) : Base(index, Class_BasePlant, create_name(15))
{
    flowers = rand() % 2;
    leaves = rand() % 2;
}

void Plant::init(BasePlant * b)
{
    base = b;
    seedling_time = 7 + rand() % 14;
    growing_time = seedling_time + rand() % 150;
    flowers_time = growing_time + rand() % 30;
    // max_age = new Property("max age", flowers_time + rand() % 100);
    phase = (Plant_phase)(rand() % (Plant_fruits + 1));
    grown = false;
    water = rand() % 100;
    // age = new Property("age", 0);
    // can_talk = false;
    switch (phase)
    {
        case Plant_seed:
            // age->value = 1;
            planted = false;
            break;
        case Plant_seedling:
            // age->value = seedling_time;
            planted = true;
            break;
        case Plant_growing:
            // age->value = growing_time;
            planted = true;
            break;
        case Plant_flowers:
            // age->value = flowers_time;
            planted = true;
            break;
        case Plant_fruits:
            // age->value = max_age->value;
            grown = true;
            planted = true;
            break;
    }
}

Plant::Plant(BasePlant * b) : InventoryElement(Class_Plant), base(b)
{
    init(b);
}
