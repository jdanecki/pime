#include "elements_server.h"
#include "../core/alchemist/random_functions.h"
#include "networking.h"
#include "world_server.h"
#include "craft_ing.h"

// #define TRACE_PLANTS 1

extern unsigned long get_time_ms();

void destroy(InventoryElement * el)
{
    if (el->location.tag == ItemLocation::Tag::Chunk)
    {
        chunk *ch = world_table[(int)el->location.chunk.map_y][(int)el->location.chunk.map_x];
        if (ch) {
            ch->remove_object(el);
            notify_destroy(el);
        }
    }
    
}

AnimalServer::AnimalServer(BaseAnimal * base) : Animal(base)
{
    delay_for_move = max_delay_move;
    delay_for_grow = max_delay_grow;
    dst_loc_x = random_range(0, CHUNK_SIZE - 1);
    dst_loc_y = random_range(0, CHUNK_SIZE - 1);
    max_age = new Property("max age", 1000, 2000);
    age = new Property("age", 10, max_age->value);
    dimensions.scale(age->value / max_age->value);
}

bool AnimalServer::action(Product_action action, Player * pl)
{
    // Animal::action(action, pl);
    CONSOLE_LOG("ANIMAL_SERVER: %s %s\n", product_action_name[action], get_name());

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
        //  notify_create(crafted);
        CONSOLE_LOG("crafted meat\n");
        destroy(this);
        return true;
    }
    else
    {
        CONSOLE_LOG("failed to craft\n");
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
    // CONSOLE_LOG("AnimalServer.grow: %llu:%llu ms delay=%d\n", ms/1000, ms % 1000, delay_for_grow);

    if (delay_for_grow)
        return false;
    delay_for_grow = max_delay_grow;

    bool ret = BeingServer::grow();
    if (!alive)
    {
        CONSOLE_LOG("%s is dead age=%d/%d\n", get_name(), age->value, max_age->value);
        destroy(this);
        // FIXME leave meat after death
    }
    else
    {
        dimensions.width.value = 2.0 * age->value / max_age->value;
        dimensions.height.value = 2.0 * age->value / max_age->value;
        dimensions.volume.value = dimensions.width.value * dimensions.height.value * dimensions.length.value;
        notify_update(this);
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

    if ((_x == dst_loc_x && _y == dst_loc_y) || (random_range(0, 5) == 1))
    {
        dst_loc_x = random_range(0, CHUNK_SIZE - 1);
        dst_loc_y = random_range(0, CHUNK_SIZE - 1);
    }
    else
    {
        if (random_range(0, 1))
        {
            if (_x < dst_loc_x)
                _x++;
            else
                _x--;
        }
        if (random_range(0, 1))
        {
            if (_y < dst_loc_y)
                _y++;
            else
                _y--;
        }
    }
    // CONSOLE_LOG("%d, %d -> dst[%d, %d]\n", _x, _y, dst_loc_x, dst_loc_y);

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

    update_location(NetworkObject(c_id, uid), l, location);
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

    set_phase(phase);
}

void PlantServer::change_phase(Plant_phase p)
{
    if (phase != p)
    {
        // CONSOLE_LOG("%s changing phase: %s -> %s age=%u/%u\n", get_name(), plant_phase_name[phase], plant_phase_name[p], age->value, max_age->value);
        notify_update(this);
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
    bool res = false;
    // CONSOLE_LOG("PLANT_SERVER: %s %s\n", player_action_name[action], get_name());
    switch (action)
    {
        case PLAYER_EAT:
            CONSOLE_LOG("ate %s\n", get_name());
            res = true;
            if (res)
            {
                pl->hunger += 10;
                destroy(this);
            }
            break;
    }

    return res;
}

void PlantServer::set_phase(Plant_phase p)
{
    phase = p;
    switch (phase)
    {
        case Plant_seedling:
            age->value = seedling_time;
            break;
        case Plant_growing:
            age->value = growing_time;
            break;
        case Plant_flowers:
            age->value = flowers_time;
            break;
        case Plant_fruits:
            age->value = max_age->value;
            grown = true;
            break;
    }
    dimensions.scale(age->value / max_age->value);
    notify_update(this);
}

bool PlantServer::grow()
{
    unsigned long ms = get_time_ms();
    //  CONSOLE_LOG("PlantServer.grow: %llu:%llu ms delay=%d\n", ms/1000, ms % 1000, delay_for_grow);
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
    dimensions.scale(age->value / max_age->value);
    notify_update(this);
    /*CONSOLE_LOG("PlantServer:%s growing %d/%d phase=%s grown=%d times=%d/%d/%d/ size=%f\n", get_name(),
        age->value, max_age->value, plant_phase_name[phase], grown,
        seedling_time, growing_time, flowers_time, size);
*/
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

IngredientServer::IngredientServer(InventoryElement * from, Ingredient_id id, Form f) : Ingredient(id)
{
    req_form = f;
    switch (from->get_cid())
    {
        case Class_Ingredient:
        {
            IngredientServer * ing = static_cast<IngredientServer *>(from);
            el = ing->el;
            break;
        }
        default:
            el = from->get_id();
            break;
    }
}

bool IngredientServer::action(Product_action action, Player * pl)
{
    CONSOLE_LOG("ING_SERVER: %s %s\n", product_action_name[action], get_name());
    return false;
}

void IngredientServer::show(bool details)
{
    CONSOLE_LOG("vvv INGREDIENT_SERVER vvv\n");
    Ingredient::show(details);
    CONSOLE_LOG("el: base_id=%d\n", el);
    CONSOLE_LOG("!!! INGREDIENT_SERVER !!!\n");
}

void ProductServer::init(int c, Form f)
{
    ing_count = c;
    req_form = f;
}

ProductServer::ProductServer(InventoryElement * el1, InventoryElement * el2, Product_id id, Form f, int act_cnt) : Product(id, act_cnt)
{
    c_id = Class_Product;
    ings[0] = (static_cast<IngredientServer *>(el1))->el;
    ings[1] = (static_cast<IngredientServer *>(el2))->el;
    init(2, f);
}

/*ProductServer::ProductServer(InventoryElement ** from, int count, Product_id id, Form f, int act_cnt) : Product(id, act_cnt)
{
    c_id = Class_Product;
    ings = (IngredientServer **)from;
    init(count, f);
}
*/
void ProductServer::show(bool details)
{
    CONSOLE_LOG("vvv PRODUCT_SERVER vvv\n");
    Product::show(details);
    for (int i = 0; i < ing_count; i++)
        CONSOLE_LOG("ings[%d] base=%d\n", i, ings[i]);
    CONSOLE_LOG("!!! PRODUCT_SERVER !!!\n");
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
    //  CONSOLE_LOG("create_element %s\n", base->get_name());
    return new ElementServer(base);
}

ScrollServer * create_scroll(Base * base)
{
    // CONSOLE_LOG("create_scroll: base=%s id=%d\n", base->get_name(), base->id);
    ScrollServer * s = new ScrollServer(base);
    return s;
}

ElementServer::ElementServer(BaseElement * b) : Element(b)
{
}
bool ElementServer::action(Product_action action, Player * pl)
{
    CONSOLE_LOG("ELEMENT_SERVER: %s %s\n", product_action_name[action], get_name());

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
    if (dimensions.volume.value < 1)
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
            dimensions.volume.value = dimensions.length.decrease(1) * dimensions.width.decrease(1) * dimensions.height.decrease(1);
            mass.value = b->density.value * dimensions.volume.value / 1000;
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
            dimensions.volume.value = dimensions.length.decrease(3) * dimensions.width.decrease(3) * dimensions.height.decrease(3);
            mass.value = b->density.value * dimensions.volume.value / 1000;
        }

        return true;
    }
    CONSOLE_LOG("ELEMENT_SERVER: action_hit failed: wrong form %s\n", Form_name[b->form]);
    return false;
}

bool ElementServer::player_action(Player_action action, Player * pl)
{
    bool res = false;

    CONSOLE_LOG("ELEMENT_SERVER: %s %s\n", player_action_name[action], get_name());
    switch (action)
    {
        case PLAYER_DRINK:
            res = action_drink();
            if (res)
            {
                pl->thirst += 10;
            }
            break;
        case PLAYER_EAT:
            res = action_eat();
            if (res)
            {
                pl->hunger += 10;
            }
            break;
    }

    if (dimensions.volume.value < 1)
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
            dimensions.volume.value = dimensions.length.decrease(2) * dimensions.width.decrease(2) * dimensions.height.decrease(2);
            mass.value = b->density.value * dimensions.volume.value / 1000;
            CONSOLE_LOG("drunk %s\n", get_name());
        }
        return true;
    }
    else
        CONSOLE_LOG("can't drink %s\n", get_name());
    return false;
}

bool ElementServer::action_eat()
{
    BaseElement * b = (BaseElement *)get_base();
    if (b->form == Form_solid)
    {
        //    if (b->solid->hardness < 50)
        {
            dimensions.volume.value = dimensions.length.decrease(4) * dimensions.width.decrease(4) * dimensions.height.decrease(4);
            mass.value = b->density.value * dimensions.volume.value / 1000;
            CONSOLE_LOG("ate %s\n", get_name());
        }
        return true;
    }
    else
        CONSOLE_LOG("can't eat %s\n", get_name());
    return false;
}

void ElementServer::show(bool details)
{
    Element::show(details);
}

bool ElementServer::can_pickup()
{
    // return mass.value < 10000;
    return true;
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
    CONSOLE_LOG("ScrollServer: %s %s\n", player_action_name[action], get_name());

    switch (action)
    {
        case PLAYER_READ:
            if (pl->set_known(get_base_cid(), get_id()))
            {
                notify_knowledge(pl->get_id(), get_base_cid(), get_id());
            }
            else
            {
                CONSOLE_LOG("ScrollServer: already known this item\n");
            }
            destroy(this);
            break;
    }

    return true;
}

bool ScrollServer::can_pickup()
{
    return true;
}
BeingServer::BeingServer()
{
    max_age = new Property("max age", 0);
    age = new Property("age", 1);
    alive = true;
}
Property ** BeingServer::get_properties(int * count)
{
    Property ** props = new Property *[2];
    props[0] = age;
    props[1] = max_age;

    *count = 2;
    return props;
}
BeingServer::~BeingServer()
{
    delete age;
    delete max_age;
}
bool BeingServer::tick()
{
    return grow();
}
bool AnimalServer::can_pickup()
{
    return true;
}

bool AnimalServer::feed()
{
    wild = false;
    return true;
}

void PlantServer::sow()
{
    change_phase(Plant_seedling);
}
bool PlantServer::action(Product_action action, Player * pl)
{
    Plant::action(action, pl);
    CONSOLE_LOG("PLANT_SERVER: %s %s\n", product_action_name[action], get_name());
    return false;
}
bool PlantServer::can_pickup()
{
    return true;
}
bool IngredientServer::can_pickup()
{
    return true;
}
bool ProductServer::use_on(InventoryElement * object, Player * pl)
{
    if (!actions_count)
        return false;
    // FIXME use more actions
    CONSOLE_LOG("%s: %s %s\n", get_name(), product_action_name[actions[0]], object->get_name());
    return object->action(actions[0], pl);
    // FIXME change properties of product after action
}
bool ProductServer::use_tile(int map_x, int map_y, int x, int y, Player * pl)
{
    if (!actions_count)
        return false;
    CONSOLE_LOG("ProductServer(%s): %s tile (%d, %d): (%d, %d)\n", get_name(), product_action_name[actions[0]], map_x, map_y, x, y);
    return true;
}
bool ProductServer::can_pickup()
{
    return true;
}
