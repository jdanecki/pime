/*
    Copyright (C) 2018  Jacek Danecki <jacek.m.danecki@gmail.com>

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include <math.h>
#include <stdlib.h>
#include "../client-common/window.h"

extern int window_width;
extern int window_height;
extern int mx, my;
float c1[3] = {0.5f, 0.5f, 0.5f};
float c2[3] = {0.5f, 0.5f, 0.5f};
float c3[3] = {1.0f, 1.0f, 1.0f};
float c4[3] = {0.0f, 0.33f, 0.66f};

#define MAX(a, b) (a > b) ? a : b
#define MIN(a, b) (a < b) ? a : b

void rgb2hsv(int r, int g, int b, int* h, int* s, int* v)
{
    float r1, g1, b1, h1=0, s1, v1;
    float cmax, cmin;
    float t, delta;

    r1 = r / 255.0;
    g1 = g / 255.0;
    b1 = b / 255.0;

    t = MAX(r1, g1);
    if (t > b1)
        cmax = t;
    else
        cmax = b1;

    t = MIN(r1, g1);
    if (t < b1)
        cmin = t;
    else
        cmin = b1;

    v1 = cmax;

    delta = cmax - cmin;
    if (delta != 0)
    {
        s1 = delta / cmax;
    }
    else
        s1 = 0.0;
    if (s1 == 0)
    {
        h1 = 0.0;
    }
    else
    {
        if (r1 == cmax)
        {
            h1 = (g1 - b1) / delta;
        }
        else
        {
            if (g1 == cmax)
            {
                h1 = 2.0 + (b1 - r1) / delta;
            }
            else
            {
                if (b1 == cmax)
                {
                    h1 = 4.0 + (r1 - g1) / delta;
                }
            }
        }
    }
    *h = (int)roundf(60.0 * h1);
    *s = (int)roundf(100 * s1);
    *v = (int)roundf(100 * v1);
}

void hsv2rgb(int h, int s, int v, int* r, int* g, int* b)
{
    // h [0..360] hue, odcień
    // s [0..100] saturation, nasycenie 0 =brak koloru (szarość), 1= pełne nasycenie (najczystszy kolor)
    // v [1..100] value, jasność 0 czarny - brak światła, 1 pełna jasność

/* H 
    0° – czerwony
    60° – żółty
    120° – zielony
    180° – cyjan
    240° – niebieski
    300° – magenta
    360° = 0° – znowu czerwony
*/    
    float r1=0, g1=0, b1=0, i, f, p, q, t, h1, s1, v1;
    s1 = s / 100.0;
    v1 = v / 100.0;

    if (!s && !h)
    {
        *r = 255 * v1;
        *g = 255 * v1;
        *b = 255 * v1;
        return;
    }
    if (h == 360) h1 = 0.0;
    h1 = h / 60.0;
    i = floor(h1);
    f = h1 - i;
    p = v1 * (1.0 - s1);
    q = v1 * (1.0 - s1 * f);
    t = v1 * (1.0 - (s1 * (1.0 - f)));
    switch ((int)i)
    {
    case 0:
        r1 = v1;
        g1 = t;
        b1 = p;
        break;
    case 1:
        r1 = q;
        g1 = v1;
        b1 = p;
        break;
    case 2:
        r1 = p;
        g1 = v1;
        b1 = t;
        break;
    case 3:
        r1 = p;
        g1 = q;
        b1 = v1;
        break;
    case 4:
        r1 = t;
        g1 = p;
        b1 = v1;
        break;
    case 5:
        r1 = v1;
        g1 = p;
        b1 = q;
        break;
    }
    *r = roundf(255 * r1);
    *g = roundf(255 * g1);
    *b = roundf(255 * b1);
}

#ifdef SHOW_PALETTE
void show_hsv_palette()
{
    int r, g, b;
    for (int s = 0; s < 100; s++)
    {
        for (int h = 0; h < 360; h++)
        {
            hsv2rgb(h, s, 100, &r, &g, &b);
            pixelRGBA(renderer, h, s, r, g, b, 255);
            
            hsv2rgb(h, 100, s, &r, &g, &b);
            pixelRGBA(renderer, h + 370, s, r, g, b, 255);            
        }
        if (mx < 360)
        {
            for (int y = 0; y < 100; y++)
            {
                hsv2rgb(mx, y, s, &r, &g, &b);
                pixelRGBA(renderer, s, 110+y, r, g, b, 255);                
            }
        }        
    }        
}

void show_rgb_palette(SDL_Texture * texture)
{
    unsigned char* pixels;
    int pitch, x, y;
    int nx, ny;

    SDL_Rect window_rec;
    SDL_LockTexture(texture, NULL, (void**)&pixels, &pitch);

    window_rec.w = window_width;
    window_rec.h = window_height;
    window_rec.x = 0;
    window_rec.y = 0;
    memset(pixels, 0, pitch * window_width);
    for (y = -255; y < 256; y++)
    {
        for (x = -255; x < 256; x++)
        {
            int r = sqrt(x * x + y * y);
            if (r < 256)
            {
                r = 255 - r;
                nx = (256 + x) << 2;
                ny = pitch * (256 + y);
                pixels[ny + nx + 2] |= r ;
                pixels[ny + nx + 3] = 255;

                nx = (384 + x) << 2;
                ny = pitch * (256 + y);
                pixels[ny + nx + 1] |= r;
                pixels[ny + nx + 3] = 255;

                nx = (320 + x) << 2;
                ny = pitch * (384 + y);
                pixels[ny + nx] |= r;
                pixels[ny + nx + 3] = 255;
            }
        }
    }
    SDL_UnlockTexture(texture);
    SDL_RenderCopy(renderer, texture, NULL, &window_rec);
}
#endif

