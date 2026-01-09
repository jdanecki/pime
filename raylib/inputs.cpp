#include <raylib.h>

#include "../client-common/inputs.h"


KeyHandler menu_key_handlers[] = {
    { KEY_ESCAPE, menu_handle_escape },
    { KEY_ENTER, menu_handle_enter},
    { KEY_DOWN, menu_go_down},
    { KEY_UP, menu_go_up},
};

KeyHandler key_handlers[] = {
    { KEY_F1, handle_f1 },
    { KEY_F2, handle_f2 },
    { KEY_F3, handle_f3 },
    { KEY_F4, handle_f4 },
    { KEY_F5, handle_f5 },
    { KEY_F6, handle_f6 },
    { KEY_F7, handle_f7 },
    { KEY_ENTER, handle_enter },
    { KEY_ONE, handle_hotbar_0 },
    { KEY_TWO, handle_hotbar_1 },
    { KEY_THREE, handle_hotbar_2 },
    { KEY_FOUR, handle_hotbar_3 },
    { KEY_FIVE, handle_hotbar_4 },
    { KEY_SIX, handle_hotbar_5 },
    { KEY_SEVEN, handle_hotbar_6 },
    { KEY_EIGHT, handle_hotbar_7 },
    { KEY_NINE, handle_hotbar_8 },
    { KEY_ZERO, handle_hotbar_9 },
    { KEY_LEFT, handle_left },
    { KEY_RIGHT, handle_right },
    { KEY_UP, handle_up },
    { KEY_DOWN, handle_down },    
    { KEY_C, handle_c },
    { KEY_I, handle_i},
#ifndef DISABLE_NPC
    { KEY_N, handle_n},
#endif
    { KEY_Q, put_element },
    { KEY_GRAVE, handle_prev_hotbar  },
    { KEY_TAB, handle_next_hotbar },
    { KEY_MINUS, handle_minus },
    { KEY_EQUAL, handle_equal },
    { KEY_ESCAPE, handle_escape },
};

void mouse_pressed(int x, int y, int button)
{
        CONSOLE_LOG("mouse %d,%d %d \n", x, y, button);
        hotbar.press(x, y, button);
        if (d_craft.show)
        {
                d_craft.press(x, y, button);
        }
}


void handle_mouse()
{
    if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
        Vector2 pos = GetMousePosition();
        mouse_pressed((int)pos.x, (int)pos.y, 1);
    }
    if (IsMouseButtonPressed(MOUSE_MIDDLE_BUTTON)) {
        Vector2 pos = GetMousePosition();
        mouse_pressed((int)pos.x, (int)pos.y, 2);
    }
    if (IsMouseButtonPressed(MOUSE_RIGHT_BUTTON)) {
        Vector2 pos = GetMousePosition();
        mouse_pressed((int)pos.x, (int)pos.y, 3);
    }
}

float last_key;
bool handle_events()
{
    //FIXME call update_window_size
 
    handle_mouse();

    float dt = GetFrameTime();
    last_key+=dt;
    
    if (last_key < 0.1) return 0;
    last_key=0;

    player_moved = false;

    int num_handlers;
    KeyHandler * handlers;

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

    if (player_moved)
    {
        print_status(0, " ");
        print_status(1, " ");
    }

    return finish_program || WindowShouldClose();
}

