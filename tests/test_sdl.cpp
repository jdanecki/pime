#include <SDL_ttf.h>
#include <errno.h>
#include <sys/stat.h>
#include "../client-common/ui/window.h"
#include "../client-common/ui/text.h"

#include "../client-common/2d/alchemist2d.h"
#include "../client-common/2d/texture.h"

bool handle_events();
Backend_Texture texture;

Element2d * el;

float w=1.0;
float h=1.0;
TTF_Font * ttf_font;

#define TTF_FONT_NAME "nerdfont.otf"
#define TTF_FONT_SIZE 128

int load_ttf_font()
 {
    TTF_Init();
     struct stat statbuf;
     int ret;
     ret = stat(TTF_FONT_NAME, &statbuf);
     if (ret)
     {
         CONSOLE_LOG("load_font(%s): %s\n", TTF_FONT_NAME, strerror(errno));
         return 1;
     }
    ttf_font = TTF_OpenFont(TTF_FONT_NAME, TTF_FONT_SIZE);
     if (!(ttf_font))
     {
        CONSOLE_LOG("load_font(%s): %s\n", TTF_FONT_NAME, TTF_GetError());
        return 1;
     }
     else
         return 0;
 }
 
void write_ttf_text(int x, int y, const char * text, SDL_Color color, int scale_x, int scale_y, bool clear_bg=false)
{
    SDL_Surface * surface;
    surface = TTF_RenderText_Solid(ttf_font, text, color);
    SDL_Texture * text_sdl = SDL_CreateTextureFromSurface(renderer, surface);


    // window_w=texture_size * CHUNK_SIZE + PANEL_WINDOW = 1084
    // window_h=texture_size * CHUNK_SIZE + STATUS_LINES = 608
    int game_size_x = 24;
    int game_size_y = 60;
    int x_size, y_size;
    x_size = strlen(text) * (scale_x ? scale_x : game_size_x);
    y_size = scale_y ? scale_y : game_size_y;

    SDL_Rect rect = {x, y, x_size, y_size};

    if (clear_bg)
    {
        SDL_SetRenderDrawColor(renderer, 10, 10, 50, 255);
        SDL_RenderFillRect(renderer, &rect);
    }

    SDL_RenderCopy(renderer, text_sdl, NULL, &rect);
    SDL_FreeSurface(surface);
    SDL_DestroyTexture(text_sdl);
 }

void test_draw()
{
    SDL_SetRenderDrawColor(renderer, 100, 100, 100, 255);
    SDL_RenderClear(renderer);
    Backend_Draw_Gradient_Rectangle(50, 100, 200, 150, Backend_Color{255, 0, 0, 255}, Backend_Color{});
    Backend_Draw_Gradient_Rectangle(300, 100, 200, 150, Backend_Color{0, 255, 0, 255}, Backend_Color{});
    Backend_Draw_Gradient_Rectangle(550, 100, 200, 150, Backend_Color{0, 0, 255, 255}, Backend_Color{});

    Backend_Draw_Gradient_Rectangle(50, 300, 200, 150, Backend_Color{}, Backend_Color{255, 0, 0, 255});
    Backend_Draw_Gradient_Rectangle(50, 300, 200, 150, Backend_Color{255, 0, 0, 255}, Backend_Color{});

    Backend_Draw_Gradient_Rectangle(300, 300, 200, 150, Backend_Color{0, 255, 0, 255}, Backend_Color{});
    Backend_Draw_Gradient_Rectangle(300, 300, 200, 150, Backend_Color{}, Backend_Color{0, 255, 0, 255});

    el->render(1, 5, w, h);

    Backend_Rect rect1(10, 10, 32, 32);
    Backend_Texture_Copy(texture, nullptr, &rect1);

    Backend_Rect rect2(50, 10, 32, 32);
    Backend_Texture_Copy_Flip(texture, nullptr, &rect2);

    Backend_Line(32, 32 * 5, 500, 32 *5, {255, 0, 0, 255});
    Backend_Line(32, 32 * 5, 32, 500, {255, 0, 0, 255});
}

bool finish_program = false;

void key_pressed(int key)
{
    switch (key)
    {
        case SDLK_ESCAPE:
            finish_program = true;
            break;
        case SDLK_LEFT:
            w-=0.1;
            if (w < 0) w = 0;
            break;
        case SDLK_RIGHT:
            w+=0.1;
            break;
        case SDLK_UP:
            h-=0.1;
            if (h < 0) h = 0;
            break;
        case SDLK_DOWN:
            h+=0.1;
            break;
        default:
            printf("key=%d\n", key);
            break;
    }
}
void mouse_pressed(SDL_MouseButtonEvent * event)
{
    printf("mouse pressed: %d,%d button: %d \n", event->x, event->y, event->button);
}

void mouse_wheel(SDL_MouseWheelEvent * event)
{
    printf("mouse wheel: %d, %d \n", event->x, event->y);
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
        if (event.type == SDL_MOUSEWHEEL)
        {
            mouse_wheel(&event.wheel);
        }
    }
    return false;
}

int main()
{
    int ret=0;
    init_window("test_sdl", 1000, 800);

    Element e(new BaseElement(Form_solid, 0));
    e.dimensions.width.value = 2;
    e.dimensions.height.value = 2;

    el = new Element2d(e);
    texture = load_texture("textures/player.png");
    if (load_ttf_font()) {
        ret=1;
        goto end;
    }

    while (!finish_program)
    {
        if (handle_events())
            return 0;

        test_draw();
        write_ttf_text(10, 520, "ABCDEFGHIJKLMNOPQRSTUVWXYZ abcdefghijklmnopqrstuvwxyz", {255, 255, 255, 255}, 13, 25);
        write_ttf_text(10, 570, "1234567890 !@#$%^&*()_+=-`~", {255, 255, 255, 255}, 13, 25);
        write_text(10, 620, "ABCDEFGHIJKLMNOPQRSTUVWXYZ abcdefghijklmnopqrstuvwxyz", {255, 255, 255, 255}, 13, 25);
        write_text(10, 670, "1234567890 !@#$%^&*()_+=-`~", {255, 255, 255, 255}, 13, 25);
        
        Backend_Update_Screen();
    }
end:
    close_graphics();
    return 0;
}
