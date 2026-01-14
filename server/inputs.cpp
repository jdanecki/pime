#include <ncursesw/ncurses.h>

#include "../core/key_handler.h"

#include "console.h"
#include "player_server.h"
#include "places/places.h"
#include "chunk.h"
#include "generator/generator.h"

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

KeyHandler key_handlers[] = {
    {KEY_RESIZE, handle_resize},
    {KEY_END, handle_end},
    {KEY_HOME, handle_home},
    {KEY_DOWN, handle_down},
    {KEY_UP, handle_up},
    {'#', handle_hash},
    {'e', handle_exit},
    {'h', print_help},
    {'c', clear_history},
    {'p', show_players},
    {'1', show_terrains},
    {'2', show_plants},
    {'3', show_animals},
    {'4', show_chunk},
    {'5', show_base_elements},
    {'6', show_base_plants},
    {'7', show_base_animals},
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
