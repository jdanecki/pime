#include "../client-common/window.h"
#include "../client-common/text.h"

int window_width;
int window_height;

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
    unload_font();
    CloseWindow();
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


