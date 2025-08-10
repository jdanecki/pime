#include <cstdio>
#include <cstdlib>
#include <cstring>

#include "../tiles.h"
#include "elements.h"
#include "names.h"

ElementsList * base_list;

const char * ingredient_name[] = {
    "Axe blade",
    "Axe handle",

    "Knife blade",
    "Knife handle",

    "Pickaxe blade",
    "Pickaxe handle",

    "Wall",
    "Meat",

    "Log",
    "Tinder",
    "Stick",
    "Fruit",
};

const char * product_name[] = {
    "Axe",
    "Knife",
    "Pickaxe",
    "Hut",
    "Fire",
    "Roasted meat",
    "Fruit Salad",
};

const char * product_action_name[] = {
    "nothing",
    "cut",
    "hit",
    "stab",
    "fire",
};
const char * player_action_name[] = {"drink", "eat", "read"};

const char * server_action_name[] = {"server show item", "server show chunk", "server trace network"};

const char * object_names[] = {"wall"};
const char * plant_phase_name[] = {"Seed", "Seedling", "Growing", "Flowers", "Fruits"};
const char * class_name[] = {"unknown", "BaseElement", "BaseAnimal", "BasePlant", "Element", "Ingredient", "Product", "Scroll", "Plant", "Animal", "Player", "Npc"};

Base::Base(int index, Class_id c, const char * name) : name(name)
{
    id = index;
    c_id = c;
}

void Base::show(bool details)
{
    printf("Base name=%s class:%s id=%d \n", get_name(), class_name[c_id], id);
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
//called by networking.cpp:create_object
Element::Element(BaseElement * b)
    //  : InventoryElement(Class_Element), base(b), length("length", 8 + rand() % 120), width("width", 8 + rand() % 120), height("height", 8 + rand() % 120),
    : InventoryElement(Class_Element), base(b), length("length", 3 + rand() % 30), width("width", 3 + rand() % 30), height("height", 3 + rand() % 30),
      volume("volume", length.value * width.value * height.value)
{
}
//called by the_game_net/core.rs
InventoryElement::InventoryElement(Class_id c_id, size_t uid, ItemLocation location) : c_id(c_id), uid(uid), location(location)
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

    printf("action: %d -> %s\n", actions, product_action_name[actions]);
}

void Animal::init(BaseAnimal * b)
{
    c_id = Class_Animal;
    size = 0;
}

BaseAnimal::BaseAnimal(int index) : Base(index, Class_BaseAnimal, create_name(7))
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

BasePlant::BasePlant(int index) : Base(index, Class_BasePlant, create_name(5))
{
    flowers = rand() % 2;
    leaves = rand() % 2;
}

void Plant::init(BasePlant * b)
{
    base = b;
    seedling_time = 10 + rand() % 20;
    growing_time = seedling_time + rand() % 150;
    flowers_time = growing_time + rand() % 30;
    size = 0;
    phase = (Plant_phase)(rand() % (Plant_fruits + 1));
    grown = false;
    water = rand() % 100;
}

Plant::Plant(BasePlant * b) : InventoryElement(Class_Plant), base(b)
{
    init(b);
}

Scroll::Scroll(BaseElement *b) : InventoryElement(Class_Scroll), base(b)
{

}

void Scroll::show(bool details)
{
    printf("%s: base=%s uid=%lx\n", get_class_name(), get_name(), uid);
}
