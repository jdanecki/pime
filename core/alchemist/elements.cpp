#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <assert.h>
#include "elements.h"

// #include "../tiles.h"
#include "el_list.h"
#include "names.h"

#ifdef USE_ENET
BaseElement * get_base_element(int32_t id);
BasePlant * get_base_plant(int32_t id);
BaseAnimal * get_base_animal(int32_t id);
#endif

ElementsList * base_list;

const char * ingredient_name[] = {
    "Axe blade",
    "Axe handle",

    "Knife blade",
    "Knife handle",

    "Pickaxe blade",
    "Pickaxe handle",

    "Hoe blade",
    "Hoe handle",

    "Wall",
    "Meat",

    "Log",
    "Tinder",
    "Stick",
    "Fruit",
    "Seed",

};

const char * product_name[] = {
    "Axe",
    "Knife",
    "Pickaxe",
    "Hoe",
    "Hut",
    "Tent",
    "Fire",
    "Roasted meat",
    "Fruit Salad",
    "Seedling",
};

const char * product_action_name[] = {
    "nothing",
    "cut",
    "hit",
    "stab",
    "fire",
    "plow",
    "plant",
    "invite",
};
const char * player_action_name[] = {"drink", "eat", "read", "check"};

const char * server_action_name[] = {"server show item", "server show chunk", "server trace network"};

const char * places_names[] = {"field"};
const char * place_states_names[] = {"plowed", "planted"};

const char * plant_phase_name[] = {"Seedling", "Growing", "Flowers", "Fruits"};
const char * class_name[] = {
    "unknown", "BaseElement", "BaseAnimal", "BasePlant", "Element", "Ingredient", "Product", "Scroll", "Place", "Plant", "Animal", "Player", "Npc", "Clan", "ListElement", "KnownElement", "BaseListElement"};

Base::Base(int index, Class_id c, const char * name) : name(name)
{
    id = index;
    c_id = c;
    edible.eating_by = 1; // only by animals
}

void Base::show(bool details)
{
    CONSOLE_LOG("Base name=%s class:%s id=%d \n", get_name(), class_name[c_id], id);
    if (details)
        edible.show();
}

const char * Base::get_name()
{
    return name.str;
}

BaseElement::BaseElement(Form f, int index) : Base(index, Class_BaseElement, create_name(5 - f)), form(f), density("", 0)
{
    // CONSOLE_LOG("BaseElement index=%d name=%s\n", index, get_name());

    switch (form)
    {
        case Form_solid:
            density = Property("density", 50 + rand() % 2000);
            color.r = rand() % 256;
            color.g = rand() % 256;
            color.b = rand() % 256;
            break;
        case Form_liquid:
            density = Property("density", 500 + rand() % 500);
            color.r = rand() % 64;
            color.g = rand() % 256;
            color.b = 255;
            break;
        case Form_gas:
            density = Property("density", 1);
            edible.set_no_edible();
            int r=rand() % 200;
            color.r = r + rand() % 56;
            color.g = r + rand() % 56;
            color.b = r + rand() % 56;
            break;
    }
}

void BaseElement::show(bool details)
{
    Base::show(details);
    CONSOLE_LOG("BaseElement form=%s\n", Form_name[form]);
    if (!details)
        return;
    density.show(); // gęstość
    CONSOLE_LOG("   form = %s color(r:%d g:%d b:%d)\n", Form_name[form], color.r, color.g, color.b);
    switch (form)
    {
        case Form_solid:
            solid.show();
            break;
        default:
            break;
    }
}

// called by networking.cpp:create_object
Element::Element(BaseElement * b)
   // : InventoryElement(Class_Element), base(b), length("length", 10 + rand() % 200), width("width", 10 + rand() % 200), height("height", 10 + rand() % 200),
    : InventoryElement(Class_Element), base(b), length("length", 5 + rand() % 60), width("width", 5 + rand() % 60), height("height", 5 + rand() % 60),
      volume("volume", length.value * width.value * height.value), sharpness("sharpness", 0), smoothness("smoothness", 0), mass("mass", b->density.value * volume.value / 1000)
{
}
#ifdef USE_ENET
Element::Element(int id) : base(get_base_element(id))
{
}
#endif

// called by the_game_net/core.rs
InventoryElement::InventoryElement(Class_id c_id, size_t uid, ItemLocation location) : NetworkObject(c_id, uid), location(location)
{
}
Class_id InventoryElement::get_cid() const
{
    return c_id;
}

size_t InventoryElement::get_uid() const
{
    return uid;
}

void Element::show(bool details)
{
    InventoryElement::show(details);

    if (!details)
        return;
    CONSOLE_LOG("form=%s\n", get_form_name());
    length.show();
    width.show();
    height.show();
    volume.show();

    if (details)
    {
        sharpness.show();
        smoothness.show();
        mass.show();
    }
    get_base()->show(details);
}

Ingredient::Ingredient(Ingredient_id i) : InventoryElement(Class_Ingredient), quality("quality", 0), resilience("resilience", 0), usage("usage", 0)
{
    id = i;
}

void Ingredient::show(bool details)
{
    InventoryElement::show(details);
    CONSOLE_LOG("name=%s id=%d\n", get_name(), id);
    if (!details)
        return;
    quality.show();
    resilience.show();
    usage.show();
    CONSOLE_LOG("form = %s", Form_name[req_form]);
}

Product::Product(Product_id id, int actions_count) : InventoryElement(Class_Product),
                                                    quality("quality", 0), resilience("resilience", 0), usage("usage", 0),
                                                    actions_count(actions_count), id(id)
{    
    c_id = Class_Product;

    assert(actions_count < 10);

    for (int i=0; i < 10; i++)
    {
        actions[i] = ACT_NOTHING;
    }
}

void Product::show(bool details)
{
    InventoryElement::show(details);
    CONSOLE_LOG("name=%s id=%d\n", get_name(), id);
    if (!details)
        return;
    quality.show();
    resilience.show();
    usage.show();
    for (int i = 0; i < actions_count; i++)
    {
        CONSOLE_LOG("action[%d]: %d -> %s\n", i,  actions[i], product_action_name[actions[i]]);
    }
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
    //  CONSOLE_LOG("BaseAnimal index=%d name=%s\n", index, get_name());
    edible.set_random();
}

Animal::Animal(BaseAnimal * b) : InventoryElement(Class_Animal), base(b)
{
    init(b);
}

#ifdef USE_ENET
Animal::Animal(int id) : base(get_base_animal(id))
{
}
#endif

BasePlant::BasePlant(int index) : Base(index, Class_BasePlant, create_name(5))
{
    flowers = rand() % 2;
    leaves = rand() % 2;
    // CONSOLE_LOG("BasePlant index=%d name=%s\n", index, get_name());
    edible.set_random();
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

#ifdef USE_ENET
Plant::Plant(int id) : base(get_base_plant(id))
{
}
#endif

Scroll::Scroll(Base * b) : InventoryElement(Class_Scroll), base(b)
{
    // CONSOLE_LOG("Scroll class=%s id=%d name=%s\n", class_name[b->c_id], b->id, b->get_name());
}

Place::Place(Place_id id, size_t uid): InventoryElement(Class_Place, uid), id(id)
{

}

Place::Place(Place_id id): InventoryElement(Class_Place), id(id)
{

}
