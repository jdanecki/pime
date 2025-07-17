#ifndef ELEMENTS_SERVER_H
#define ELEMENTS_SERVER_H

#include "../../core/alchemist/elements.h"

class BaseElementServer : public BaseElement
{
    public:
    Property * density;
    Solid * solid;

    BaseElementServer(Form f, int index);
    ~BaseElementServer();
    int foo(int a);
    void show(bool details = true);
};

void to_bytes_binding(InventoryElement * el, unsigned char * buf);
unsigned int get_packet_size_binding(InventoryElement * el);

extern "C"
{
    void update_location(size_t id, ItemLocation old_loc, ItemLocation new_loc);
    void notify_destroy(size_t id, ItemLocation location);
}

void destroy(InventoryElement * el);

const int max_delay_move = 10;  // 1 sec.
const int max_delay_grow = 600; // 1 min.

class ElementServer : public Element
{

  public:
    Property sharpness;
    Property smoothness;

    ElementServer(BaseElement* base);    
    bool action(Product_action action);
    bool action_cut();
    bool action_hit();


};

class BeingServer
{
  public:
    Property * age;
    Property * max_age;
    bool alive;
    BeingServer()
    {
        max_age = new Property("max age", 1 + rand() % 36000); // 100 years
        age = new Property("age", rand() % max_age->value);
    }
    Property ** get_properties(int * count)
    {
        Property ** props = new Property *[2];
        props[0] = age;
        props[1] = max_age;

        *count = 2;
        return props;
    }
    ~BeingServer()
    {
        delete age;
        delete max_age;
    }
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
            // printf("%s is dying\n", get_name()); // FIXME
        }
        return alive;
    }
    bool tick()
    {
        return grow();
    }
};

class AnimalServer : public Animal
{
    int delay_for_move;
    int dst_loc_x, dst_loc_y;

  public:
    void move();
    bool tick() override;
    // AnimalServer();
    AnimalServer(BaseAnimal* base);
    bool action(Product_action action)
    {
        Animal::action(action);
        printf("ANIMAL_SERVER: %s %s\n", Product_action_names[action], get_name());
        return false;
    }

};

class PlantServer : public Plant, public BeingServer
{
    int * padding; // FIXME
    int delay_for_grow;

  public:
    bool grow() override;
    // bool tick() override;
    // PlantServer();
    PlantServer(BasePlant* base);

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
            printf("%s changing phase: %s -> %s age=%u/%u\n", get_name(), Plant_phase_name[phase], Plant_phase_name[p], age->value, max_age->value);
        }
        phase = p;
    }
    bool action(Product_action action)
    {
        Plant::action(action);
        printf("PLANT_SERVER: %s %s\n", Product_action_names[action], get_name());
        return false;
    }
};

class IngredientServer : public Ingredient
{
    
public:

    
    InventoryElement * el; // available only in server , move to IngredientServer class
    bool craft();
    IngredientServer(InventoryElement * from, Ingredient_id i, Form f);
};

class ProductServer : public Product
{
public:
        
    int ing_count;
    InventoryElement ** ings;
    bool craft() override;

    void init(Product_id i, int c, Form f);
    ProductServer(InventoryElement * el1, InventoryElement * el2, Product_id i, Form f);
    ProductServer(InventoryElement ** from, int count, Product_id i, Form f);
};

AnimalServer* create_animal(BaseAnimal* base);
PlantServer* create_plant(BasePlant* base);
ElementServer *create_element(BaseElement* base);

#endif
