#include "elements_server.h"
#include "networking.h"
#include "world_server.h"
#include "craft_ing.h"

// #define TRACE_PLANTS 1

extern unsigned long get_time_ms();

void destroy(InventoryElement * el)
{
    notify_destroy(el->get_uid(), el->location);
    if (el->location.tag == ItemLocation::Tag::Chunk)
    {
        world_table[el->location.chunk.map_y][el->location.chunk.map_x]->remove_object(el);
    }
    delete el;
}

AnimalServer::AnimalServer(BaseAnimal * base) : Animal(base)
{
    delay_for_move = max_delay_move;
    delay_for_grow = max_delay_grow;
    dst_loc_x = rand() % CHUNK_SIZE;
    dst_loc_y = rand() % CHUNK_SIZE;
    max_age = new Property("max age", 100 + rand() % 1000);
    age = new Property("age", 10 + rand() % max_age->value);
    size = 1.0 * age->value / max_age->value;
}

bool AnimalServer::action(Product_action action, Player * pl)
{
    // Animal::action(action, pl);
    printf("ANIMAL_SERVER: %s %s\n", product_action_name[action], get_name());

    InventoryElement * crafted = nullptr;
    switch (action)
    {
        case ACT_CUT:
        case ACT_HIT:
        case ACT_STAB:
            crafted = craft_ing((int)ING_MEAT, this);
            break;
        default:
            return false;
    }
    if (crafted)
    {
        add_object_to_world(crafted, pl->location);
        objects_to_create.add(crafted);
        printf("crafted meat\n");
        destroy(this);
        return true;
    }
    else
    {
        printf("failed to craft\n");
    }
    return false;
}

void AnimalServer::show(bool details)
{
    Animal::show(details);
    BeingServer::show(details);
}

bool AnimalServer::grow()
{
    delay_for_grow--;
    // unsigned long ms=get_time_ms();
    // printf("AnimalServer.grow: %llu:%llu ms delay=%d\n", ms/1000, ms % 1000, delay_for_grow);

    if (delay_for_grow)
        return false;
    delay_for_grow = max_delay_grow;

    bool ret = BeingServer::grow();
    if (!alive)
    {
        printf("%s is dead age=%d/%d\n", get_name(), age->value, max_age->value);
        destroy(this);
        // FIXME leave meat after death
    }
    else
    {
        size = 1.0 * age->value / max_age->value;
        notify_update(this);
        //   printf("%s:%s growing size=%f %d/%d \n", get_class_name(), get_name(), size, age->value, max_age->value);
    }
    return ret;
}

void AnimalServer::move()
{
    delay_for_move--;
    if (delay_for_move)
        return;
    delay_for_move = max_delay_move;

    ItemLocation l = location;
    int _x = location.chunk.x;
    int _y = location.chunk.y;

    if ((_x == dst_loc_x && _y == dst_loc_y) /*|| (rand() % 5 ==1)*/)
    {
        dst_loc_x = rand() % CHUNK_SIZE;
        dst_loc_y = rand() % CHUNK_SIZE;
    }
    else
    {
        if (rand() % 2)
        {
            if (_x < dst_loc_x)
                _x++;
            else
                _x--;
        }
        if (rand() % 2)
        {
            if (_y < dst_loc_y)
                _y++;
            else
                _y--;
        }
    }
    //  printf("%d, %d -> dst[%d, %d]\n", _x, _y, dst_loc_x, dst_loc_y);

    if (_x >= CHUNK_SIZE)
        _x = CHUNK_SIZE - 1;
    if (_y >= CHUNK_SIZE)
        _y = CHUNK_SIZE - 1;
    if (_y < 0)
        _y = 0;
    if (_y < 0)
        _y = 0;
    if (_x < 0)
        _x = 0;

    location.chunk.x = _x;
    location.chunk.y = _y;

    update_location(uid, l, location);
}

bool AnimalServer::tick()
{
    move();
    return BeingServer::tick();
}

PlantServer::PlantServer(BasePlant * base) : Plant(base)
{
    delay_for_grow = max_delay_grow;
    max_age->value = flowers_time + 50;

    switch (phase)
    {
        case Plant_seed:
            age->value = 1;
            planted = false;
            size = 0.01;
            break;
        case Plant_seedling:
            age->value = seedling_time;
            planted = true;
            size = 0.01;
            break;
        case Plant_growing:
            age->value = growing_time;
            planted = true;
            size = 1.0 * age->value / max_age->value;
            break;
        case Plant_flowers:
            age->value = flowers_time;
            planted = true;
            size = 1.0;
            break;
        case Plant_fruits:
            age->value = max_age->value;
            grown = true;
            planted = true;
            size = 1.0;
            break;
    }
}

void PlantServer::change_phase(Plant_phase p)
{
    notify_update(this);
    if (phase != p)
    {
        //   printf("%s changing phase: %s -> %s age=%u/%u\n", get_name(), plant_phase_name[phase], plant_phase_name[p], age->value, max_age->value);
    }
    phase = p;
}

void PlantServer::show(bool details)
{
    Plant::show(details);
    BeingServer::show(details);
}

bool PlantServer::player_action(Player_action action, Player * pl)
{
    printf("PLANT_SERVER: %s %s\n", player_action_name[action], get_name());
    return false;
}

bool PlantServer::grow()
{
    // unsigned long ms=get_time_ms();
    // printf("PlantServer.grow: %llu:%llu ms delay=%d\n", ms/1000, ms % 1000, delay_for_grow);
    if (grown)
        return false;

    delay_for_grow--;
    if (delay_for_grow)
        return false;
    delay_for_grow = max_delay_grow;

    // if (!water)
    //   return !grown;
    // water--;

    age->value++;
    size = 1.0 * age->value / max_age->value;
#ifdef TRACE_PLANTS
    printf("PlantServer:%s growing %d/%d phase=%s grown=%d planted=%d times=%d/%d/%d/ size=%f\n", get_name(), age->value, max_age->value, plant_phase_name[phase], grown, planted, seedling_time,
        growing_time, flowers_time, size);
#endif
    if (age->value >= max_age->value)
    {
        if (phase != Plant_fruits)
        {
            grown = true;
            change_phase(Plant_fruits);
        }
        return !grown;
    }
    if (age->value >= flowers_time)
    {
        if (phase != Plant_flowers)
        {
            change_phase(Plant_flowers);
        }
        return !grown;
    }
    if (age->value >= growing_time)
    {
        if (phase != Plant_growing)
        {
            change_phase(Plant_growing);
        }

        return !grown;
    }
    if (age->value >= seedling_time)
    {
        if (phase != Plant_seedling)
        {
            change_phase(Plant_seedling);
        }
        return !grown;
    }
    return !grown;
}

IngredientServer::IngredientServer(InventoryElement * from, Ingredient_id i, Form f) : Ingredient(i)
{
    c_id = Class_Ingredient;
    el = from;
    id = i;
    req_form = f;
}

bool IngredientServer::action(Product_action action, Player * pl)
{
    printf("ING_SERVER: %s %s\n", product_action_name[action], get_name());
    return false;
}

void ProductServer::init(Product_id i, int c, Form f)
{
    id = i;
    ing_count = c;
    req_form = f;
}

ProductServer::ProductServer(InventoryElement * el1, InventoryElement * el2, Product_id i, Form f) : Product(i)
{
    c_id = Class_Product;
    ings = (InventoryElement **)calloc(2, sizeof(InventoryElement));
    ings[0] = el1;
    ings[1] = el2;
    init(i, 2, f);
}

ProductServer::ProductServer(InventoryElement ** from, int count, Product_id i, Form f) : Product(i)
{
    c_id = Class_Product;
    ings = from;
    init(i, count, f);
}

void ProductServer::show(bool details)
{
    Product::show(details);
}

AnimalServer * create_animal(BaseAnimal * base)
{
    return new AnimalServer(base);
}

PlantServer * create_plant(BasePlant * base)
{
    return new PlantServer(base);
}

ElementServer * create_element(BaseElement * base)
{
    //   printf("create_element %s\n", base->get_name());
    return new ElementServer(base);
}

ScrollServer * create_scroll(Base * base)
{
    //  printf("create_scroll: base=%s id=%d\n", base->get_name(), base->id);
    ScrollServer * s = new ScrollServer(base);
    return s;
}

ElementServer::ElementServer(BaseElement * b) : Element(b)
{
    if (b->form == Form_solid)
    {
        sharpness.value = rand() % 100;
        smoothness.value = rand() % 100;
    }
}

bool ElementServer::action(Product_action action, Player * pl)
{
    printf("ELEMENT_SERVER: %s %s\n", product_action_name[action], get_name());

    bool res = false;
    switch (action)
    {
        case ACT_CUT:
            res = action_cut();
            break;
        case ACT_HIT:
            res = action_hit();
            break;
    }
    if (volume.value < 1)
    {
        destroy(this);
    }
    else
    {
        notify_update(this);
    }
    return res;
}

bool ElementServer::action_cut()
{
    BaseElement * b = (BaseElement *)get_base();
    if (b->form == Form_solid)
    {
        //    if (b->solid->hardness < 50)
        {
            volume.value = length.decrease(1) * width.decrease(1) * height.decrease(1);
            mass.value = b->density.value * volume.value / 1000;
        }

        return true;
    }
    return false;
}

bool ElementServer::action_hit()
{
    BaseElement * b = (BaseElement *)get_base();
    if (b->form == Form_solid)
    {
        //    if (b->solid->hardness < 50)
        {
            volume.value = length.decrease(3) * width.decrease(3) * height.decrease(3);
            mass.value = b->density.value * volume.value / 1000;
        }

        return true;
    }
    return false;
}

bool ElementServer::player_action(Player_action action, Player * pl)
{
    bool res = false;

    printf("ELEMENT_SERVER: %s %s\n", player_action_name[action], get_name());
    switch (action)
    {
        case PLAYER_DRINK:
            res = action_drink();
            if (res)
                pl->thirst += 10;
            break;
        case PLAYER_EAT:
            res = action_eat();
            if (res)
                pl->hunger += 10;
            break;
    }

    if (volume.value < 1)
    {
        destroy(this);
    }
    else
    {
        notify_update(this);
    }
    return res;
}

bool ElementServer::action_drink()
{
    BaseElement * b = (BaseElement *)get_base();
    if (b->form == Form_liquid)
    {
        //    if (b->solid->hardness < 50)
        {
            volume.value = length.decrease(2) * width.decrease(2) * height.decrease(2);
            mass.value = b->density.value * volume.value / 1000;
            printf("drunk %s\n", get_name());
        }
        return true;
    }
    else
        printf("can't drink %s\n", get_name());
    return false;
}

bool ElementServer::action_eat()
{
    BaseElement * b = (BaseElement *)get_base();
    if (b->form == Form_solid)
    {
        //    if (b->solid->hardness < 50)
        {
            volume.value = length.decrease(4) * width.decrease(4) * height.decrease(4);
            mass.value = b->density.value * volume.value / 1000;
            printf("ate %s\n", get_name());
        }
        return true;
    }
    else
        printf("can't eat %s\n", get_name());
    return false;
}

void ElementServer::show(bool details)
{
    Element::show((details));
}

bool ElementServer::can_pickup()
{
    return mass.value < 10000;
}

void BeingServer::show(bool details)
{
    age->show();
    max_age->show();
}

bool BeingServer::grow()
{
    if (!alive)
    {
        return false;
    }
    age->value++;

    if (age->value >= max_age->value)
    {
        alive = false;
    }
    return alive;
}

ScrollServer::ScrollServer(Base * b) : Scroll(b)
{
}

bool ScrollServer::player_action(Player_action action, Player * pl)
{
    printf("ScrollServer: %s %s\n", player_action_name[action], get_name());

    switch (action)
    {
        case PLAYER_READ:
            if (pl->set_known(get_base_cid(), get_id()))
            {
                notify_knowledge(pl->get_id(), get_base_cid(), get_id());
            }
            else
            {
                printf("ScrollServer: already known this item\n");
            }
            destroy(this);
            break;
    }

    return true;
}
