#ifndef TEXT_H
#define TEXT_H

#include "backend.inl"

extern Backend_Font font;
extern Backend_Color White;
extern Backend_Color Red;
extern Backend_Color Gray;
extern Backend_Color Cyan;
extern Backend_Color Yellow;

extern char status_line[256];
extern char status_line2[256];
extern void print_status(int i, const char * format, ...);
extern void write_text(int x, int y, const char * text, Backend_Color color, int scale_x, int scale_y);

extern int load_font();
extern void unload_font();

#endif
