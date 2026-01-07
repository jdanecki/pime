#include "../client-common/text.h"

#define FONT_NAME "nerdfont.otf"

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
