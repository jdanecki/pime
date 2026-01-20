#ifndef KEY_HANDLERS_H
#define KEY_HANDLERS_H

#include <ncursesw/ncurses.h>
#include "../core/key_handler.h"
#include "console.h"

extern struct KeyHandler key_handlers[];
extern bool finish_program;
bool handle_pressed();

#endif // KEY_HANDLERS_H
