#ifndef TEXT_H
#define TEXT_H

#include "backend.inl"

#define FONT_NAME "nerdfont.otf"
#define INITIAL_FONT_SIZE 128

extern Backend_Font font;
extern Backend_Color White;
extern Backend_Color Red;
extern Backend_Color Gray;
extern Backend_Color Cyan;
extern Backend_Color Yellow;

void write_text(int x, int y, const char * text, Backend_Color color, int scale_x, int scale_y, bool clear_bg = false);
void write_text(int x, int y, const char * text, Backend_Color color);

int load_font();
void unload_font();

#endif
