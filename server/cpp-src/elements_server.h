#ifndef ELEMENTS_SERVER_H
#define ELEMENTS_SERVER_H

#include "../../core/alchemist/elements.h"
#include "../../core//player.h"

void to_bytes_binding(InventoryElement * el, unsigned char * buf);
unsigned int get_packet_size_binding(InventoryElement * el);

extern "C"
{
    void notify_update(const InventoryElement * el);
    void update_location(size_t id, ItemLocation old_loc, ItemLocation new_loc);
    void notify_destroy(size_t id, ItemLocation location);
    void notify_knowledge(size_t pl_id, Class_id cid, int id);
    void notify_checked(size_t pl_id, size_t el);
}

void destroy(InventoryElement * el);

const int max_delay_move = 10; // 1 sec.
const int max_delay_grow = 10; // 600=1 min.

constexpr static const unsigned long TICK_DELAY = 100;

class ElementServer : public Element
{
  public:
    ElementServer(BaseElement * b);
    bool action(Product_action action, Player * pl) override;
    bool action_cut();
    bool action_hit();

    bool player_action(Player_action action, Player * pl) override;
    bool action_drink();
    bool action_eat();
    void show(bool details = true) override;
    bool can_pickup() override;
};

class ScrollServer : public Scroll
{
  public:
    ScrollServer(Base * base);
    bool can_pickup() override
    {
        return true;
    }
    bool player_action(Player_action action, Player * pl) override;
};

class BeingServer
{
  public:
    Property * age;
    Property * max_age;
    bool alive;
    int padding;
    BeingServer()
    {
        max_age = new Property("max age", 0);
        age = new Property("age", 1);
        alive = true;
    }
    void show(bool details = true);
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
    virtual bool grow();
    virtual bool tick()
    {
        return grow();
    }
};

class AnimalServer : public Animal, public BeingServer
{
    int delay_for_move;
    int delay_for_grow;
    int dst_loc_x, dst_loc_y;
    int padding1;

  public:
    void move();
    bool tick() override;

    AnimalServer(BaseAnimal * base);
    bool action(Product_action action, Player * pl) override;
    void show(bool details = true) override;
    bool grow() override;
    bool can_pickup() override
    {
        return true;
    }
};

class PlantServer : public Plant, public BeingServer
{
    int * padding; // FIXME
    int delay_for_grow;

  public:
    bool grow() override;

    PlantServer(BasePlant * base);

    void sow()
    {
        planted = 1;
        change_phase(Plant_seedling);
    }
    void change_phase(Plant_phase p);
    bool action(Product_action action, Player * pl) override
    {
        Plant::action(action, pl);
        CONSOLE_LOG("PLANT_SERVER: %s %s\n", product_action_name[action], get_name());
        return false;
    }
    void show(bool details = true) override;
    bool can_pickup() override
    {
        return true;
    }
    bool player_action(Player_action action, Player * pl);
};

class IngredientServer : public Ingredient
{

  public:
    InventoryElement * el;
    IngredientServer(InventoryElement * from, Ingredient_id i, Form f);
    bool action(Product_action action, Player * pl) override;
    bool can_pickup() override
    {
        return true;
    }
};

class ProductServer : public Product
{
  public:
    int ing_count;
    InventoryElement ** ings;

    void init(Product_id i, int c, Form f);
    ProductServer(InventoryElement * el1, InventoryElement * el2, Product_id i, Form f);
    ProductServer(InventoryElement ** from, int count, Product_id i, Form f);
    void show(bool details = true) override;
    bool use(InventoryElement * object, Player * pl)
    {
        // if (!actions) return false;
        if (actions == ACT_NOTHING)
            return false;
        CONSOLE_LOG("%s: %s %s\n", get_name(), product_action_name[actions], object->get_name());
        return object->action(actions, pl);
        // FIXME change properties of product after action
    }
    bool can_pickup() override
    {
        return true;
    }
};

AnimalServer * create_animal(BaseAnimal * base);
PlantServer * create_plant(BasePlant * base);
ElementServer * create_element(BaseElement * base);
ScrollServer * create_scroll(Base * base);

#endif
