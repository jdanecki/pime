#include <sys/stat.h>
#include <unistd.h>

#include "../client-common/window.h"
#include "../client-common/text.h"
#include "../client-common/texture.h"

int window_width;
int window_height;

int init_window(const char * title, int wx, int wy)
{
    InitWindow(wx, wy, title);

    window_width = GetScreenWidth();
    window_height = GetScreenHeight();
    SetTargetFPS(60);
    tile_size = 32;
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

    load_textures();

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
