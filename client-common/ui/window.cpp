#include "../core/world_params.h"
#include "dialog/d_hotbar.h"

#include "window.h"
#include "../2d/texture.h"

void set_tile_size(int width, int height)
{
    int h = height - STATUS_LINES - HOTBAR_HEIGHT;
    if (width < h)
    {
        tile_size = width / CHUNK_SIZE;
    }
    else
    {
        tile_size = h / CHUNK_SIZE;
    }
    if (tile_size < 10)
        tile_size = 10;
    CONSOLE_LOG("tile_size=%d\n", tile_size);
}
