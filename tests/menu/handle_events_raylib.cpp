#include <raylib.h>
#include "../../menu/menu.h"
#include "../../client-common/key_handler.h"

extern bool show_craft;
extern bool finish_program;

void handle_mouse(int x, int y, int button);
void handle_escape();
void handle_i();
void quit_program();

KeyHandler menu_key_handlers[] = {
    { KEY_ESCAPE, menu_handle_escape },
    { KEY_ENTER, menu_handle_enter},
    { KEY_DOWN, menu_go_down},
    { KEY_UP, menu_go_up},
};

KeyHandler key_handlers[] = {
    { KEY_ESCAPE, handle_escape },
    { KEY_I, handle_i},
#ifndef DISABLE_NPC
    { KEY_N, handle_n},
#endif
    { KEY_Q, quit_program}
};

void check_mouse()
{
    if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
        Vector2 pos = GetMousePosition();
        handle_mouse((int)pos.x, (int)pos.y, 1);
    }
    if (IsMouseButtonPressed(MOUSE_MIDDLE_BUTTON)) {
        Vector2 pos = GetMousePosition();
        handle_mouse((int)pos.x, (int)pos.y, 2);
    }
    if (IsMouseButtonPressed(MOUSE_RIGHT_BUTTON)) {
        Vector2 pos = GetMousePosition();
        handle_mouse((int)pos.x, (int)pos.y, 3);
    }
}

float last_key;
bool handle_events()
{
    check_mouse();

    int num_handlers;
    KeyHandler * handlers;

    float dt = GetFrameTime();
    last_key+=dt;

    if (last_key < 0.1) return 0;
    last_key=0;

    if (current_menu) {
        num_handlers = sizeof(menu_key_handlers)/sizeof(KeyHandler);
        handlers= menu_key_handlers;
    }
    else {
        num_handlers = sizeof(key_handlers)/sizeof(KeyHandler);
        handlers= key_handlers;
    }
    for (int i = 0; i < num_handlers; ++i) {
        if (IsKeyDown(handlers[i].key)) {
            handlers[i].func();
        }
    }

    return WindowShouldClose() || finish_program;
}

