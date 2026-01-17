#include "key_handlers.h"
#include "elements_server.h"
#include "player_server.h"
#include "places/places.h"
#include "chunk.h"
#include "generator/generator.h"
#include "tools/tools.h"
#include "console.h"

bool debug_mode;

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

void debug_create_place()
{
    chunk * ch = world_table[128][128];
    PlaceServer * p = create_place(PLACE_FIELD);
    ch->add_object(p, 9, 9);
}

void debug_create_seedling()
{
    chunk * ch = world_table[128][128];
    Seedling * seedl=create_seedling();
    ch->add_object(seedl, 10, 9);
}

void debug_create_hoe()
{
    chunk * ch = world_table[128][128];
    Hoe * hoe=create_hoe();
    if (hoe) ch->add_object(hoe, 11, 9);
}

void debug_print_help()
{
    CONSOLE_LOG("F1 - help\n");
    CONSOLE_LOG("F2 - switch_debug_mode\n");
    CONSOLE_LOG("p - create place\n");
    CONSOLE_LOG("s - create seedling\n");
    CONSOLE_LOG("h - create hoe\n");
    CONSOLE_LOG("1 - create all base elements\n");
    CONSOLE_LOG("2 - create all base plants\n");
    CONSOLE_LOG("3 - create all animals\n");
}

void print_help()
{
    CONSOLE_LOG("F1 - help\n");
    CONSOLE_LOG("F2 - switch_debug_mode\n");
    CONSOLE_LOG("F3 - clear\n");
    CONSOLE_LOG("F12 - exit\n");
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
void switch_debug_mode()
{
   debug_mode ^= true;
   print_status(0, "debug_mode=%d press F1 for help       ", debug_mode);
   if (debug_mode) debug_print_help(); else print_help();
}

void create_all_base_elements()
{
    chunk * ch = world_table[128][128];
    for (size_t id=0; id < all_base_elements_count; id++)
    {
        BaseListElement * ble = (BaseListElement *)base_elements.find(&id);
        BaseElement * bel = (BaseElement *)(ble->get_el());
        ElementServer *el = create_element(bel);
        ch->add_object(el, 3*(id % 6), 3*(id / 6));
        notify_create(el);
    }
}

void create_all_base_plants()
{
    chunk * ch = world_table[128][128];
    for (size_t id = 0; id < BASE_PLANTS; id++)
    {
        BaseListElement * ble = (BaseListElement *)base_plants.find(&id);
        BasePlant * bp = (BasePlant *)(ble->get_el());
        PlantServer *pl= create_plant(bp);
        ch->add_object(pl, id % CHUNK_SIZE, 10 + 3 * (id / CHUNK_SIZE));
        notify_create(pl);
    }
}

void create_all_base_animals()
{
    chunk * ch = world_table[128][128];

    for (size_t id=0; id < BASE_ANIMALS; id++)
    {
        BaseListElement * ble = (BaseListElement *)base_animals.find(&id);
        BaseAnimal *ba=(BaseAnimal *)(ble->get_el());
        AnimalServer *an = create_animal(ba);
        ch->add_object(an, id % CHUNK_SIZE, 15 + id / CHUNK_SIZE);
        notify_create(an);
    }
}

KeyHandler debug_key_handlers[] = {
    {KEY_F(1), debug_print_help},
    {KEY_F(2), switch_debug_mode},
    {'p', debug_create_place},
    {'s', debug_create_seedling},
    {'h', debug_create_hoe},
    {'1', create_all_base_elements},
    {'2', create_all_base_plants},
    {'3', create_all_base_animals},
};


KeyHandler key_handlers[] = {
    {KEY_RESIZE, handle_resize},
    {KEY_END, handle_end},
    {KEY_HOME, handle_home},
    {KEY_DOWN, handle_down},
    {KEY_UP, handle_up},
    {KEY_F(1), print_help},
    {KEY_F(2), switch_debug_mode},
    {KEY_F(3), clear_history},
    {KEY_F(12), handle_exit},
    {'p', show_players},

    {'1', show_terrains},
    {'2', show_plants},
    {'3', show_animals},
    {'4', show_chunk},
    {'5', show_base_elements},
    {'6', show_base_plants},
    {'7', show_base_animals},
    {'#', handle_hash},
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

    int num_handlers;
    KeyHandler * handlers;
    if (debug_mode)
    {
        num_handlers= sizeof(debug_key_handlers) / sizeof(KeyHandler);
        handlers = debug_key_handlers;
    }
    else {
        num_handlers= sizeof(key_handlers) / sizeof(KeyHandler);
        handlers = key_handlers;
    }

    for (int i = 0; i < num_handlers; ++i)
    {
        if (pressed == handlers[i].key)
        {
            handlers[i].func();
            return finish_program;
        }
    }
    return false;
}
