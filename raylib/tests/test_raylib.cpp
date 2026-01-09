#include <stdio.h>

#include "../../client-common/window.h"
#include "../../client-common/alchemist2d.h"

bool handle_events();

BasePlant * get_base_plant(size_t id)
{
    printf("get_base_plant id=%ld\n", id);
    return nullptr;
}
BaseAnimal * get_base_animal(size_t id)
{
    printf("get_base_animal id=%ld\n", id);
    return nullptr;
}

BaseElement * get_base_element(size_t id)
{
    printf("get_base_element id=%ld\n", id);
    return nullptr;
}

Element2d *el;

void draw()
{
   Color c;
   c.r=100;
   c.g=100;
   c.b=100;
   c.a=255;

    Backend_Begin_Drawing();
    ClearBackground(c);

    Backend_Draw_Gradient_Rectangle(50, 100, 200, 150, Backend_Color{255, 0, 0, 255},  Backend_Color{});
    Backend_Draw_Gradient_Rectangle(300, 100, 200, 150, Backend_Color{0, 255, 0, 255}, Backend_Color{});
    Backend_Draw_Gradient_Rectangle(550, 100, 200, 150, Backend_Color{0, 0, 255, 255}, Backend_Color{});

    Backend_Draw_Gradient_Rectangle(50, 300, 200, 150, Backend_Color{},  Backend_Color{255,0,0,255});
    Backend_Draw_Gradient_Rectangle(50, 300, 200, 150, Backend_Color{255, 0, 0, 255}, Backend_Color{});

    Backend_Draw_Gradient_Rectangle(300, 300, 200, 150, Backend_Color{0,255,0,255}, Backend_Color{});
    Backend_Draw_Gradient_Rectangle(300, 300, 200, 150, Backend_Color{}, Backend_Color{0,255,0,255});

    el->render(10, 500);

    Backend_Update_Screen();
    Backend_End_Drawing();
}

bool finish_program = false;

void mouse_pressed(int x, int y, int button)
{
    printf("mouse %d,%d %d \n", x, y, button);
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

bool handle_events()
{
    handle_mouse();

    if (IsKeyDown(KEY_ESCAPE)) return true;

    return false;
}

void unload_font() {}


int main()
{
   init_window("test_raylib", 1000, 800);

   Element e(new BaseElement(Form_solid, 0));
    e.width.value=100;
    e.height.value=100;

    el = new Element2d(e);

   while(!finish_program)
   {
        if (handle_events())
            return 0;

        draw();
    }

  close_graphics();
  return 0;
}
