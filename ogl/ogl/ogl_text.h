/*
    pime-ogl - an OpenGL implementation of a pime client
    Copyright (C) 2026 Piotr Danecki <i3riced@mailfence.com>

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <https://www.gnu.org/licenses/>.
*/
#pragma once

#include <GL/gl.h>
#include <SDL3/SDL.h>
#include <cassert>

class OGL_Text
{
    GLuint texture;
    int char_width;
    int char_height;
    int chars_per_row;
    int first_char;
    int texture_width;
    int texture_height;

    void draw_char(char c, float x, float y, float scale)
    {
        int char_index = c - first_char;
        if (char_index < 0)
            return;

        int col = char_index % chars_per_row;
        int row = char_index / chars_per_row;

        float u1 = (float)(col * char_width) / texture_width;
        float v1 = (float)(row * char_height) / texture_height;
        float u2 = (float)((col + 1) * char_width) / texture_width;
        float v2 = (float)((row + 1) * char_height) / texture_height;

        float w = char_width * scale;
        float h = char_height * scale;

        glBindTexture(GL_TEXTURE_2D, texture);
        glBegin(GL_QUADS);
        glTexCoord2f(u1, v1);
        glVertex2f(x, y);
        glTexCoord2f(u2, v1);
        glVertex2f(x + w, y);
        glTexCoord2f(u2, v2);
        glVertex2f(x + w, y + h);
        glTexCoord2f(u1, v2);
        glVertex2f(x, y + h);
        glEnd();
    }

  public:
    OGL_Text(const char * image_path, int char_width, int char_height, int chars_per_row, int first_char)
        : char_width(char_width), char_height(char_height), chars_per_row(chars_per_row), first_char(first_char)
    {
        SDL_Surface * surface = SDL_LoadPNG(image_path);
        if (!surface)
            assert("Couldn't open font file" == 0);

        glGenTextures(1, &texture);
        glBindTexture(GL_TEXTURE_2D, texture);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        GLenum format = (SDL_BYTESPERPIXEL(surface->format) == 4) ? GL_RGBA : GL_RGB;
        glTexImage2D(GL_TEXTURE_2D, 0, format, surface->w, surface->h, 0, format, GL_UNSIGNED_BYTE, surface->pixels);

        texture_width = surface->w;
        texture_height = surface->h;

        SDL_DestroySurface(surface);
    }

    void setup_2d_projection(int screen_width, int screen_height)
    {
        glMatrixMode(GL_PROJECTION);
        glPushMatrix();
        glLoadIdentity();
        glOrtho(0, screen_width, screen_height, 0, -1, 1);
        glMatrixMode(GL_MODELVIEW);
        glPushMatrix();
        glLoadIdentity();
        glDisable(GL_DEPTH_TEST);
        glDisable(GL_CULL_FACE);
    }

    void restore_3d_projection()
    {
        glEnable(GL_DEPTH_TEST);
        glEnable(GL_CULL_FACE);
        glMatrixMode(GL_PROJECTION);
        glPopMatrix();
        glMatrixMode(GL_MODELVIEW);
        glPopMatrix();
    }

    void draw_text(const char * text, float x, float y, float scale, int screen_width, int screen_height)
    {
        setup_2d_projection(screen_width, screen_height);
        float cursor_x = x;

        glPushAttrib(GL_ENABLE_BIT | GL_COLOR_BUFFER_BIT | GL_CURRENT_BIT);
        glEnable(GL_TEXTURE_2D);
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        glColor4f(1.0f, 1.0f, 1.0f, 1.0f);

        for (int i = 0; text[i] != '\0'; i++)
        {
            if (text[i] == '\n')
            {
                cursor_x = x;
                y += char_height * scale;
                continue;
            }
            draw_char(text[i], cursor_x, y, scale);
            cursor_x += char_width * scale;
        }

        glPopAttrib();
        restore_3d_projection();
    }
};
