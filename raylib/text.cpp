#include <string.h>
#include <stdio.h>

#include "../client-common/text.h"

#define FONT_NAME "nerdfont.otf"

Color White = {255, 255, 255, 255};
Color Gray = {200, 200, 200, 255};
Color Red = {255, 0, 0, 255};
Color Cyan = {255, 0, 255, 255};
Color Yellow = {255, 255, 0, 255};

Font font;

int load_font()
{
    font = LoadFont(FONT_NAME);
    return 0;
}

void unload_font()
{
    UnloadFont(font);
}

void write_text(int x, int y, const char * text, Color c, int /*scale_x*/, int scale_y)
{
    Vector2 position = { (float)x, (float)y };
    int window_w, window_h;
    window_w=GetScreenWidth();
    window_h=GetScreenHeight();

    int game_size;

    if (window_w < window_h)
    {
        game_size = window_w;
    }
    else
    {
        game_size = window_h;
    }

//    int x_size;
    int y_size;
  //  x_size = strlen(text) * (scale_x ? scale_x : game_size / 25);
    y_size = scale_y ? scale_y : game_size / 10;

 //   printf("text: %s\n", text);
    DrawTextEx(font, text, position, y_size, 0, c);
}
