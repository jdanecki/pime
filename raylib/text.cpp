#include <string.h>
#include <stdio.h>

#include "../client-common/text.h"

Color White = {255, 255, 255, 255};
Color Gray = {200, 200, 200, 255};
Color Red = {255, 0, 0, 255};
Color Cyan = {255, 0, 255, 255};
Color Yellow = {255, 255, 0, 255};

Font font;

int load_font()
{
    //    font = LoadFont(FONT_NAME);
    font = LoadFontEx(FONT_NAME, INITIAL_FONT_SIZE, NULL, 0);
    SetTextureFilter(font.texture, TEXTURE_FILTER_POINT);
    //    SetTextureFilter(font.texture, TEXTURE_FILTER_BILINEAR);
    //    SetTextureFilter(font.texture, TEXTURE_FILTER_TRILINEAR);
    return 0;
}

void unload_font()
{
    UnloadFont(font);
}

void write_text(int x, int y, const char * text, Color c, int scale_x, int scale_y, bool clear_bg)
{
    Vector2 position = {(float)x, (float)y};
    int window_w, window_h;
    window_w = GetScreenWidth();
    window_h = GetScreenHeight();

    int game_size;

    if (window_w < window_h)
    {
        game_size = window_w;
    }
    else
    {
        game_size = window_h;
    }

    int x_size;
    int y_size;
    x_size = strlen(text) * (scale_x ? scale_x : game_size / 25);
    y_size = scale_y ? scale_y : game_size / 10;

    if (clear_bg)
    {
        Color bc;
        bc.r = 10;
        bc.g = 10;
        bc.b = 50;
        bc.a = 255;
        Rectangle r{(float)x, (float)y, (float)x_size, (float)y_size};
        DrawRectangleRec(r, bc);
    }

    DrawTextEx(font, text, position, y_size, 0, c);
}
