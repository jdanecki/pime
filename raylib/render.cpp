#include <raylib.h>

#include "playerUI.h"
#include "../client-common/game.h"

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

