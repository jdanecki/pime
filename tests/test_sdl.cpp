#include "../client-common/window.h"
#include "../client-common/alchemist2d.h"

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
    SDL_SetRenderDrawColor(renderer, 100, 100, 100, 255);
    SDL_RenderClear(renderer);
    Backend_Draw_Gradient_Rectangle(50, 100, 200, 150, Backend_Color{255, 0, 0, 255},  Backend_Color{});
    Backend_Draw_Gradient_Rectangle(300, 100, 200, 150, Backend_Color{0, 255, 0, 255}, Backend_Color{});
    Backend_Draw_Gradient_Rectangle(550, 100, 200, 150, Backend_Color{0, 0, 255, 255}, Backend_Color{});

    Backend_Draw_Gradient_Rectangle(50, 300, 200, 150, Backend_Color{},  Backend_Color{255,0,0,255});
    Backend_Draw_Gradient_Rectangle(50, 300, 200, 150, Backend_Color{255, 0, 0, 255}, Backend_Color{});

    Backend_Draw_Gradient_Rectangle(300, 300, 200, 150, Backend_Color{0,255,0,255}, Backend_Color{});
    Backend_Draw_Gradient_Rectangle(300, 300, 200, 150, Backend_Color{}, Backend_Color{0,255,0,255});

    el->render(10, 500);
}

bool finish_program = false;

void key_pressed(int key)
{
    switch (key)
    {
        case SDLK_ESCAPE: finish_program = true; break;
        default:
            printf("key=%d\n", key);
        break;
    }
}
void mouse_pressed(SDL_MouseButtonEvent * event)
{
    printf("mouse %d,%d %d \n", event->x, event->y, event->button);
}

bool handle_events()
{
    SDL_Event event;

    while (SDL_PollEvent(&event))
    {
        if (event.type == SDL_QUIT)
        {
            return true;
        };
        if (event.type == SDL_KEYDOWN)
        {
            int key = event.key.keysym.sym;
            key_pressed(key);
        }
        if (event.type == SDL_MOUSEBUTTONDOWN)
        {
            mouse_pressed(&event.button);
        }
    }
    return false;
}

int main()
{
   init_window("test_sdl", 1000, 800);

   Element e(new BaseElement(Form_solid, 0));
    e.width.value=100;
    e.height.value=100;

    el = new Element2d(e); ;

   while(!finish_program)
   {
        if (handle_events())
            return 0;

        draw();
        Backend_Update_Screen();
    }

  close_graphics();
  return 0;
}
