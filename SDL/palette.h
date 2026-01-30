#ifndef PALETTE_H
#define PALETTE_H

void rgb2hsv(int r, int g, int b, int * h, int * s, int * v);
void hsv2rgb(int h, int s, int v, int * r, int * g, int * b);

#ifdef SHOW_PALETTE
void show_palette(int pal);
#endif

#endif