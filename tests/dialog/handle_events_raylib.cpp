#include <raylib.h>

extern bool show_craft;

void handle_mouse(int x, int y, int button);

bool handle_events()
{
    if (IsKeyDown(KEY_ESCAPE)) {
         if (show_craft) {
                show_craft=0;
                return false;
            } 
        }
    if (IsKeyDown(KEY_Q)) {             
             return true;
    }
    
    if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
        Vector2 pos = GetMousePosition();
        handle_mouse((int)pos.x, (int)pos.y, 1);
    }
    if (IsMouseButtonPressed(MOUSE_RIGHT_BUTTON)) {
        Vector2 pos = GetMousePosition();
        handle_mouse((int)pos.x, (int)pos.y, 3);
    }
    if (IsMouseButtonPressed(MOUSE_MIDDLE_BUTTON)) {
        Vector2 pos = GetMousePosition();
        handle_mouse((int)pos.x, (int)pos.y, 2);
    }

    return WindowShouldClose();

}

