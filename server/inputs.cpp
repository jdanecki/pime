#include <ncursesw/ncurses.h>

#include "../core/key_handler.h"

#include "console.h"
#include "elements_server.h"
#include "player_server.h"
#include "places/places.h"
#include "chunk.h"
#include "generator/generator.h"
#include "tools/tools.h"

void handle_end()
{
    scrolling = true;
    history_up = buf_pos - win_h + 1;
    if (history_up < 0)
        history_up = 0;
}
void handle_home()
{
    scrolling = false;
    history_up = 0;
}
void handle_down()
{
    scrolling = false;
    if (history_up < (buf_pos - 5))
        history_up += 5;
}
void handle_up()
{
    scrolling = false;
    history_up -= 5;
    if (history_up < 0)
        history_up = 0;
}

void handle_hash()
{
    show_key ^= 1;
}

bool finish_program;
void handle_exit()
{
    finish_program = true;
}

void print_help()
{
    CONSOLE_LOG("e - exit\n");
    CONSOLE_LOG("h - help\n");
    CONSOLE_LOG("c - clear\n");
    CONSOLE_LOG("p - players\n");
    CONSOLE_LOG("1 - show terrains types\n");
    CONSOLE_LOG("2 - show plants types\n");
    CONSOLE_LOG("3 - show animals types\n");
    CONSOLE_LOG("4 - show chunk\n");
    CONSOLE_LOG("5 - show base elements\n");
    CONSOLE_LOG("6 - show base plants\n");
    CONSOLE_LOG("7 - show base animals\n");
    CONSOLE_LOG("# - show keys\n");
}

void show_base_elements()
{
    base_elements.show();
}

void show_base_plants()
{
    base_plants.show();
}

void show_base_animals()
{
    base_animals.show();
}

void create_debug_element()
{
    chunk * ch = world_table[128][128];
    PlaceServer * p = create_place(PLACE_FIELD);
    ch->add_object(p, 9, 9);

    size_t b_id = 0;
    BaseListElement * base_el = static_cast<BaseListElement *>(base_plants.find(&b_id));
    PlantServer * plant1 = create_plant((BasePlant *)(base_el->get_el()));
    plant1->phase = Plant_fruits;

    BaseListElement * base_el1 = static_cast<BaseListElement *>(base_plants.find(&b_id));
    PlantServer * plant2 = create_plant((BasePlant *)(base_el1->get_el()));
    plant2->phase = Plant_fruits;

    IngredientServer *s1 = Seed::createSeed(plant1);
    IngredientServer *s2 = Seed::createSeed(plant2);

    ProductServer *seedl = Seedling::createSeedling(s1, s2);
    ch->add_object(seedl, 10, 9);

    int count;
    NetworkObject ** base_solid = base_elements.find_form(Form_solid, &count);
    CONSOLE_LOG("count=%d \n", count);
    if (count)
    {
        for (int i=0; i < count; i++)
        {
            BaseElement *base=static_cast<BaseElement*>(base_solid[i]);
            CONSOLE_LOG("solid: %d/%d\n", i, count);
//            base->show(false);
        }

        ElementServer *el1=create_element(static_cast<BaseElement *>(base_solid[0]));
        ElementServer *el2=create_element(static_cast<BaseElement *>(base_solid[1]));
        IngredientServer *hb=HoeBlade::createHoeBlade(el1);
        IngredientServer *hh=HoeHandle::createHoeHandle(el2);
        ProductServer *hoe = Hoe::createHoe(hb, hh);
        ch->add_object(hoe, 9, 10);
    }
}

void update_debug_element()
{

/*  size_t b_id = 0;
    BaseListElement * base_el = (BaseListElement *)base_plants.find(&b_id);
    PlantServer * plant = create_plant((BasePlant *)(base_el->base));
    ch->add_object(plant, x, y);
    notify_create(plant);*/

    chunk * ch = world_table[128][128];
    for (InventoryElement * el : ch->objects)
    {
        if (el->c_id == Class_Place) {
            Place *p = (Place*) el;
            p->state = FIELD_PLANTED;
            notify_update(p);
        }
    }
}

KeyHandler key_handlers[] = {
    {KEY_RESIZE, handle_resize},
    {KEY_END, handle_end},
    {KEY_HOME, handle_home},
    {KEY_DOWN, handle_down},
    {KEY_UP, handle_up},
    {'#', handle_hash},
    {'c', clear_history},
    {'e', handle_exit},
    {'h', print_help},
    {'p', show_players},

    {'1', show_terrains},
    {'2', show_plants},
    {'3', show_animals},
    {'4', show_chunk},
    {'5', show_base_elements},
    {'6', show_base_plants},
    {'7', show_base_animals},

    {'d', create_debug_element},
    {'u', update_debug_element},
};

bool handle_pressed()
{
    int pressed = wgetch(in_w);
    if (show_key)
    {
        if (pressed != ERR)
            CONSOLE_LOG("key=%d ", pressed);
    }
    if (pressed == ERR || (!pressed))
        return false;

    int num_handlers = sizeof(key_handlers) / sizeof(KeyHandler);
    for (int i = 0; i < num_handlers; ++i)
    {
        if (pressed == key_handlers[i].key)
        {
            key_handlers[i].func();
        }
    }
    return finish_program;
}
