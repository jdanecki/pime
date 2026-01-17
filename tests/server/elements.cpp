#include <stdarg.h>
#include <stdio.h>

#include "../../server/elements_server.h"
#include "../../server/networking.h"
#include "../../server/tools/tools.h"

void notify_destroy(NetworkObject id, ItemLocation location) {}
void notify_create(const InventoryElement * el) {}
void notify_update(const InventoryElement * el) {}
void update_location(NetworkObject id, ItemLocation old_loc, ItemLocation new_loc) {}
void notify_knowledge(size_t pl_id, Class_id cid, int id) {}

Networked::Networked() {}

void print_status(int , const char * format, ...)
{
    va_list args;
    va_start(args, format);
    vprintf(format, args);
    va_end(args);
}

int CONSOLE_LOG(const char * fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    vprintf(fmt, args);
    va_end(args);
    return 0;
}

int main()
{
    for (int i=0; i < 5; i++)
    {
        Form f = Form_solid;
        ListElement * entry = new BaseListElement(new BaseElement(f, i));
        base_elements.add(entry);
    }
    for (int i = 0; i < 5; i++)
    {
        ListElement * entry = new BaseListElement(new BasePlant(i));
        base_plants.add(entry);
    }
    for (int i = 0; i < 5; i++)
    {
        ListElement * entry = new BaseListElement(new BaseAnimal(i));
        base_animals.add(entry);
    }

    Seedling * s1=create_seedling();
    printf("Seedling1: id=%ld\n", s1->get_id());
    printf("ing[0]=%ld\n", s1->ings[0]->get_base_id());
    printf("ing[1]=%ld\n", s1->ings[1]->get_base_id());

    size_t b_id=3;
    BaseListElement * base_el = (BaseListElement *)base_plants.find(&b_id);
    PlantServer * plant1= create_plant((BasePlant *)(base_el->get_el()));
    plant1->set_phase(Plant_fruits);
    IngredientServer * fruit1=Fruit::createFruit(plant1);

    PlantServer * plant2= create_plant((BasePlant *)(base_el->get_el()));
    plant2->set_phase(Plant_fruits);
    IngredientServer * fruit2=Fruit::createFruit(plant2);

    IngredientServer *seed1=Seed::createSeed(fruit1);
    IngredientServer *seed2=Seed::createSeed(fruit2);

    ProductServer * s2=Seedling::createSeedling(seed1, seed2);
    printf("Seedling2: id=%ld\n", s2->get_id());
    printf("ing[0]=%ld\n", s2->ings[0]->get_base_id());
    printf("ing[1]=%ld\n", s2->ings[1]->get_base_id());

    return 0;
}
