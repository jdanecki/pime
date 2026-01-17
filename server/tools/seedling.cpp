#include "tools.h"
#include "../../core/world.h"

extern ElementsList base_plants;

Seedling::Seedling(InventoryElement * el1, InventoryElement * el2) : ProductServer(el1, el2, PROD_SEEDLING, Form_solid, 1)
{
    actions[0] = ACT_PLANT;
}

ProductServer * Seedling::createSeedling(InventoryElement * el1, InventoryElement * el2)
{
    int id1 = el1->get_id();
    int id2 = el2->get_id();

    if ((id1 == ING_SEED && id2 == ING_SEED))
        return new Seedling(el1, el2);
    CONSOLE_LOG(" wrong ingredients id1=%d id=%d\n", id1, id2);
    return nullptr;
}

bool Seedling::use(InventoryElement * object, Player * pl)
{
    unsigned int map_x, map_y;
    unsigned int x, y;

    map_x = object->location.chunk.map_x;
    map_y = object->location.chunk.map_y;
    x = object->location.get_x();
    y = object->location.get_y();

    if (object->c_id != Class_Place)
    {
        CONSOLE_LOG("It's not a place to plow\n");
        return false;
    }
    Place * p = (Place *)object;
    if (p->get_id() != PLACE_FIELD)
    {
        CONSOLE_LOG("It's not a field\n");
        return false;
    }
    if (p->state != FIELD_PLOWED)
    {
        CONSOLE_LOG("Field isn't plowed\n");
        return false;
    }
    CONSOLE_LOG("%s: %s on %s @[%d,%d][%d,%d]\n", get_name(), product_action_name[actions[0]], object->get_name(), map_x, map_y, x, y);

    chunk * ch = world_table[map_y][map_x];
    IngredientServer * ing = (IngredientServer *)(ings[0]);
    size_t b_id = ing->get_base_id();
    CONSOLE_LOG("%s: ing base=%d\n", get_name(), b_id);
    BaseListElement * base_el = (BaseListElement *)base_plants.find(&b_id);
    PlantServer * plant = create_plant((BasePlant *)(base_el->get_el()));
    plant->set_phase(Plant_seedling);
    CONSOLE_LOG("%s: planted base=%d\n", get_name(), plant->get_id());
    ch->add_object(plant, x, y);
    notify_create(plant);
    p->state = FIELD_PLANTED;
    notify_update(p);

    return true;
}

Seedling * create_seedling()
{
    size_t b_id = rand() % base_plants.nr_elements;
    BaseListElement * base_el = static_cast<BaseListElement *>(base_plants.find(&b_id));
    PlantServer * plant1 = create_plant((BasePlant *)(base_el->get_el()));
    plant1->phase = Plant_fruits;

    BaseListElement * base_el1 = static_cast<BaseListElement *>(base_plants.find(&b_id));
    PlantServer * plant2 = create_plant((BasePlant *)(base_el1->get_el()));
    plant2->phase = Plant_fruits;

    IngredientServer *s1 = Seed::createSeed(plant1);
    IngredientServer *s2 = Seed::createSeed(plant2);

    return static_cast<Seedling*>(Seedling::createSeedling(s1, s2));
}
