#include <sys/ioctl.h>
#include <termios.h>
#include <time.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <termios.h>
#include <unistd.h>
#include "../core/time_core.h"
#include "../client-common/game.h"
#include <raylib.h>

int window_width;
int window_height;

#define FONT_NAME "nerdfont.otf"
Font font;

void load_textures()
{}


void create_menus()
{}


int load_font()
{
    font = LoadFont(FONT_NAME);
    return 0;
}

int init_window(const char * title, int wx, int wy)
{
   InitWindow(wx, wy, title);

    window_width=GetScreenWidth();
    window_height=GetScreenHeight();
    SetTargetFPS(60);
    return 0;
}

void close_graphics()
{
    UnloadFont(font);
    CloseWindow();
}

void print_status(int l, const char * format, ...)
{
    va_list args;
    va_start(args, format);
    if (!l)
        vprintf(format, args);
    else
        vprintf(format, args);
    va_end(args);
    puts("");
}


bool handle_events()
{
    return WindowShouldClose();
}

void clear_window()
{
   Color c;
   c.r=0;
   c.g=0;
   c.b=200;
   c.a=255;

   ClearBackground(c);
}

void draw()
{
    for (int y = -1; y < 2; y++)
    {
        for (int x = -1; x < 2; x++)
        {
            check_chunk(player->location.chunk.map_x + x, player->location.chunk.map_y + y);
        }
    }

    BeginDrawing();

    DrawFPS(10, 10);

    EndDrawing();


    if (!auto_explore)
        WaitTime(0.2);
}

int main(int argc, char * argv[])
{
    start_game("pime_raylib", argc, argv);
}
