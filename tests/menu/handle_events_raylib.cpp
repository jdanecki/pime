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
    { KEY_ESCAPE, menu_handle_escape, true},
    { KEY_ENTER, menu_handle_enter, true},
    { KEY_DOWN, menu_go_down, false},
    { KEY_UP, menu_go_up, false},
};

KeyHandler key_handlers[] = {
    { KEY_ESCAPE, handle_escape, true },
    { KEY_I, handle_i, true},
#ifndef DISABLE_NPC
    { KEY_N, handle_n, true},
#endif
    { KEY_Q, quit_program, true}
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

    if (current_menu) {
        num_handlers = sizeof(menu_key_handlers)/sizeof(KeyHandler);
        handlers= menu_key_handlers;
    }
    else {
        num_handlers = sizeof(key_handlers)/sizeof(KeyHandler);
        handlers= key_handlers;
    }
    for (int i = 0; i < num_handlers; ++i) {
        if (handlers[i].press_key)
        {
            if (IsKeyPressed(handlers[i].key)) {
                handlers[i].func();
                break;
            }
        }
        else {
            if (IsKeyDown(handlers[i].key)) {
                if (last_key < 0.15) break;
                last_key=0;
                handlers[i].func();
                break;
            }
        }
    }

    return WindowShouldClose() || finish_program;
}

