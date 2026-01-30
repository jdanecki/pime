#include <sys/stat.h>
#include <unistd.h>

#include "../client-common/ui/window.h"
#include "../client-common/ui/text.h"

int window_width;
int window_height;

int init_window(const char * title, int wx, int wy, bool resizable)
{
    InitWindow(wx, wy, title);

    window_width = GetScreenWidth();
    window_height = GetScreenHeight();
    CONSOLE_LOG("window_width=%d window_height=%d\n", window_width, window_height);

    SetTargetFPS(60);

    SetExitKey(KEY_NULL);
    SetTraceLogLevel(LOG_WARNING);
    //    SetTraceLogLevel(LOG_NONE);
    if (load_font())
        return 1;

    struct stat statbuf;
    int ret = stat("textures", &statbuf);
    if (ret)
    {
        chdir("..");
        ret = stat("textures", &statbuf);
        if (ret)
        {
            CONSOLE_LOG("missing directory with textures\n");
            return 2;
        }
    }

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
    c.r = 10;
    c.g = 10;
    c.b = 50;
    c.a = 255;

    ClearBackground(c);
}
