#include "elements_server.h"
#include "networking.h"
#include "world_server.h"

#ifndef CORE_TUI
void destroy(InventoryElement * el)
{
    notify_destroy(el->get_uid(), el->location);
    if (el->location.tag == ItemLocation::Tag::Chunk)
    {
        world_table[el->location.chunk.map_y][el->location.chunk.map_x]->remove_object(el);
    }
    delete el;
}
#endif

BaseElementServer::BaseElementServer(Form f, int index) : BaseElement(f, {rand()%256, rand()%256, rand()%256},index)
{
    solid = NULL;
    
    density = nullptr;
    form = f;
    switch (f)
    {
        case Form_solid:
            solid = new Solid;
            density = new Property("density", 50 + rand() % 2000);
            break;
        case Form_liquid:
            density = new Property("density", 500 + rand() % 500);
            break;
        case Form_gas:
            density = new Property("density", 1);
            break;
    }

    name = create_name(5 - form);
}

BaseElementServer::~BaseElementServer() 
{
    delete density;
    if (solid)
        delete solid;
}

int BaseElementServer::foo(int a)
{
    return a+1;
}

void BaseElementServer::show(bool details)
{
    Base::show(details);
    printf("BaseElement form=%s\n", Form_name[form]);
    if (!details)
        return;
    density->show(); // gęstość
    printf("   form = %s\n", Form_name[form]);
    switch (form)
    {
        case Form_solid:
            solid->show();
            break;
        default:
            break;
    }
}

// AnimalServer::AnimalServer()
// {
//     delay_for_move = max_delay_move; // 600 * 100ms -> 1min
//     dst_loc_x = rand() % CHUNK_SIZE;
//     dst_loc_y = rand() % CHUNK_SIZE;
// }

AnimalServer::AnimalServer(BaseAnimal* base) : Animal(base)
{
    delay_for_move = max_delay_move; // 600 * 100ms -> 1min
    dst_loc_x = rand() % CHUNK_SIZE;
    dst_loc_y = rand() % CHUNK_SIZE;
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
    // return Being::tick();
    return 1;
}

// PlantServer::PlantServer()
// {
//     delay_for_grow = max_delay_grow;
// }

PlantServer::PlantServer(BasePlant* base) : Plant(base)
{
    delay_for_grow = max_delay_grow;
}

bool PlantServer::grow()
{
    delay_for_grow--;
    if (delay_for_grow)
        return false;
    delay_for_grow = max_delay_grow;

    if (grown)
        return false;
    if (!water)
        return !grown;
    // water--;
    age->value++;

    if (age->value >= max_age->value)
    {
        if (phase != Plant_fruits)
        {
            grown = true;
            change_phase(Plant_fruits);
#ifndef CORE_TUI
            objects_to_update.add(this);
#endif
        }
        return !grown;
    }
    if (age->value >= flowers_time)
    {
        if (phase != Plant_flowers)
        {
            change_phase(Plant_flowers);
#ifndef CORE_TUI
            objects_to_update.add(this);
#endif
        }
        return !grown;
    }
    if (age->value >= growing_time)
    {
        if (phase != Plant_growing)
        {
            change_phase(Plant_growing);
#ifndef CORE_TUI
            objects_to_update.add(this);
#endif
        }
        return !grown;
    }
    if (age->value >= seedling_time)
    {
        if (phase != Plant_seedling)
        {
            change_phase(Plant_seedling);
#ifndef CORE_TUI
            objects_to_update.add(this);
#endif
        }
        return !grown;
    }
    return !grown;
}
/*
bool PlantServer::tick()
{
    grow();
    Plant::tick();
    return true;
}*/


IngredientServer::IngredientServer(InventoryElement * from, Ingredient_id i, Form f) : Ingredient(i)
{
    c_id = Class_Ingredient;
    el = from;
    id = i;
    req_form = f;
}
bool IngredientServer::craft()
{
    // FIXME
    // if (req_form != el->get_form())
    // {
    //     printf("form != %d\n", req_form);
    //     return false;
    // }

    // quality = Property("quality", rand() % 100);
    // resilience = Property("resilience", rand() % 100);
    // usage = Property("usage", rand() % 100);
    return true;
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

bool ProductServer::craft() // executed only on server
{
    for (int i = 0; i < ing_count; i++)
    {
        // TODO fixme
        // if (req_form != ings[i]->get_form())
        // {
        //     printf("form != %d for inq[%d]\n", req_form, i);
        //     return false;
        // }
    }
    if (!check_ing())
        return false;

    quality = Property("quality", rand() % 100);
    resilience = Property("resilience", rand() % 100);
    usage = Property("usage", rand() % 100);
    return true;
}

AnimalServer* create_animal(BaseAnimal* base)
{
    return new AnimalServer(base);
}

PlantServer* create_plant(BasePlant* base)
{
    return new PlantServer(base);
}

ElementServer* create_element(BaseElement* base)
{
    return new ElementServer(base);
}

ElementServer::ElementServer(BaseElement *base):Element(base),
      sharpness("sharpness", rand() %100),
      smoothness("smoothness", rand() %100)
{

}

bool ElementServer::action(Product_action action)
{
    printf("ELEMENT_SERVER: %s %s\n", Product_action_names[action], get_name());
    printf("%s\n", get_description());
    bool res = false;
    switch(action)
    {
        case ACT_CUT:     res = action_cut(); break;
        case ACT_HIT:     res = action_hit(); break;
    }
    if (volume.value < 1)
    {
        destroy(this);
    }
    else
    {
        objects_to_update.add(this);
    }
    return res;
}

bool ElementServer::action_cut()
{
    BaseElementServer *b=(BaseElementServer*) get_base();
    if (b->form == Form_solid)
    {
        //    if (b->solid->hardness < 50)
        {
            length.value/=2;
            width.value/=2;
            height.value/=2;
            volume.value=length.value * width.value * height.value;
        }
        printf("%s\n", get_description());
        return true;
    }
    return false;
}

bool ElementServer::action_hit()
{
     BaseElementServer *b=(BaseElementServer*) get_base();
    if (b->form == Form_solid)
    {
        //    if (b->solid->hardness < 50)

        {
            length.value/=4;
            width.value/=4;
            height.value/=4;
            volume.value=length.value * width.value * height.value;
        }
        printf("%s\n", get_description());
        return true;
    }
    return false;
}
