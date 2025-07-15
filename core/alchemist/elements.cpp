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

Base::Base(int index, Class_id c, const char* name) : name(name)
{
    id = index;
    c_id = c;
}

void Base::show(bool details)
{
    printf("Base name=%s class:%d id=%d \n", get_name(), c_id, id);
}

const char* Base::get_name() 
{
    return name.str;
}
    
BaseElement::BaseElement(Form f, Color color, int index) : Base(index, Class_BaseElement, create_name(5-f)), form(f), color(color)
{
}

template<typename T>
SerializablePointer<T>::SerializablePointer(T* p) : ptr(p) {}

Element::Element(BaseElement * b) : InventoryElement(Class_Element), base(b)
{

}
InventoryElement::InventoryElement(Class_id c_id, size_t uid, unsigned int mass, ItemLocation location) : c_id(c_id), uid(uid), mass(mass), location(location)
{
    
}
void Element::show(bool details)
 {
    printf("%s: base=%s form=%s uid=%lx\n", Class_names[c_id], base.get()->get_name(), get_form_name(), uid);
     if (!details)
         return;

     base.get()->show(details);
 }

Ingredient::Ingredient(Ingredient_id i) : InventoryElement(Class_Ingredient),
                                          quality("quality", 0),
                                          resilience("resilience", 0),
                                          usage("usage", 0)
{
    id = i;
}

void Ingredient::show(bool details)
{
    printf("%s -> class:%d id=%d\n", Ingredient_name[id], c_id, id);
    if (!details)
        return;
    quality.show();
    resilience.show();
    usage.show();
    printf("form = %s", Form_name[req_form]);
}

Product::Product(Product_id i) : InventoryElement(Class_Product),
      quality("quality", 0), resilience("resilience", 0) , usage("usage", 0)
{
    id = i;
    c_id = Class_Product;    
}

void Product::show(bool details)
{
    printf("%s -> class:%d id=%d\n", Product_name[id], c_id, id);
    if (!details)
        return;
    quality.show();
    resilience.show();
    usage.show();
}

void init_elements()
{
    // base_list = new ElementsList("base list");

    // BaseTable * base_elements = new BaseTable(BASE_ELEMENTS, Class_BaseElement);

    // int i = 0;

    // for (; i < SOLID_ELEMENTS; i++)
    // {
    //     base_elements->add(i, new BaseElement(Form_solid, i));
    // }
    // for (; i < SOLID_ELEMENTS + LIQUID_ELEMENTS; i++)
    // {
    //     base_elements->add(i, new BaseElement(Form_liquid, i));
    // }
    // for (; i < SOLID_ELEMENTS + LIQUID_ELEMENTS + GAS_ELEMENTS; i++)
    // {
    //     base_elements->add(i, new BaseElement(Form_gas, i));
    // }
    // base_list->add(base_elements);

    // BaseTable * base_animals = new BaseTable(BASE_ANIMALS, Class_BaseAnimal);

    // for (i = 0; i < BASE_ANIMALS; i++)
    // {
    //     base_animals->add(i, new BaseAnimal(i));
    // }
    // base_list->add(base_animals);

    // BaseTable * base_plants = new BaseTable(BASE_PLANTS, Class_BasePlant);

    // for (i = 0; i < BASE_PLANTS; i++)
    // {
    //     base_plants->add(i, new BasePlant(i));
    // }
    // base_list->add(base_plants);
}

void show_base_table(Class_id id, bool details)
{
    BaseTable * table = dynamic_cast<BaseTable *>(base_list->find(&id));
    table->show(details);
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

Animal::Animal(BaseAnimal * b) : InventoryElement(Class_Animal),  base(b){
    init(b);
}

// Animal::Animal()
// {
//     Class_id id = Class_BaseAnimal;
//     BaseTable * table = dynamic_cast<BaseTable *>(base_list->find(&id));
//     init(dynamic_cast<BaseAnimal *>(table->get_random()));
// }

// Animal::Animal(int i)
// {
//     Class_id id = Class_BaseAnimal;
//     BaseTable * table = dynamic_cast<BaseTable *>(base_list->find(&id));
//     init(dynamic_cast<BaseAnimal *>(table->get(i)));
// }

BasePlant::BasePlant(int index) : Base(index, Class_BasePlant, create_name(15))
{
    flowers = rand() % 2;
    leaves = rand() % 2;
}

// Plant::Plant()
// {
//     Class_id id = Class_BasePlant;
//     BaseTable * table = dynamic_cast<BaseTable *>(base_list->find(&id));
//     init(dynamic_cast<BasePlant *>(table->get_random()));
// }

// Plant::Plant(int i)
// {
//     Class_id id = Class_BasePlant;
//     BaseTable * table = dynamic_cast<BaseTable *>(base_list->find(&id));
//     init(dynamic_cast<BasePlant *>(table->get(i)));
// }

// FIXME split Plant to sertver/client side
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

// BaseElement* get_base_element(int id)
// {
//     BaseTable* table = (BaseTable*)base_list->head;
//     return (BaseElement*)table->get(id);
// }
