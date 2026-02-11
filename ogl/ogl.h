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
#include "../core/world_params.h"
#include "../core/alchemist/elements/element.h"
#include <GL/gl.h>
#include <SDL3/SDL.h>
#include <cassert>
#include <cmath>
#include <cstdio>
#include <unordered_map>

typedef struct OGL_Camera
{
    float x, y, z;
    float pitch, yaw;
    // ----- yaw
    //
    // |
    // | pitch
    // |
    float fov;
    float znear, zfar;

    OGL_Camera() : x(128 * CHUNK_SIZE + 8.5), y(2), z(128 * CHUNK_SIZE + 8.5), pitch(0), yaw(0), fov(90), znear(0.1), zfar(1000)
    {
    }

    static void load_perspective(float fovy_deg, float aspect, float znear, float zfar)
    {
        float fovy = fovy_deg * (float)M_PI / 180.0f;
        float f = 1.0f / tanf(fovy * 0.5f);
        float nf = 1.0f / (znear - zfar);

        // Column-major order for glMultMatrixf (OpenGL expects column-major)
        float m[16] = {f / aspect, 0, 0, 0, 0, f, 0, 0, 0, 0, (zfar + znear) * nf, -1, 0, 0, (2.0f * zfar * znear) * nf, 0};

        glMultMatrixf(m);
    }

    void begin_camera(SDL_Window * window)
    {
        int w, h;
        SDL_GetWindowSize(window, &w, &h);
        glViewport(0, 0, w, (h > 0 ? h : 1));

        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();
        load_perspective(90.0f, (float)w / (float)(h > 0 ? h : 1), 0.1f, 2000.0f);
        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();

        glRotatef(-pitch, 1.0f, 0.0f, 0.0f);
        glRotatef(yaw, 0.0f, 1.0f, 0.0f);
        glTranslatef(-x, -y, -z);
    }

    const char * get_direction_string()
    {
        int dir = int(round((yaw) / 90.0)) % 4;
        switch (dir)
        {
            case 0:
                return "North (-Z)";
            case 1:
                return "East (+X)";
            case 2:
                return "South (+Z)";
            case 3:
                return "West (-X)";
        }
        return "Unknown?";
    }
} OGL_Camera;

typedef struct OGL_Vertex
{
    float x, y, z;
    float nx, ny, nz;
    float u, v;
    GLuint tex_id;
} OGL_Vertex;

typedef struct OGL_Plane
{
    GLuint texture;
    int r, g, b;
    OGL_Vertex vertices[6];
    OGL_Plane(float x, float y, float z, GLuint texture, int r, int g, int b) : texture(texture), r(r), g(g), b(b)
    {
        update_vertices(x, y, z);
    };

    void update_vertices(float _x, float _y, float _z)
    {
        int idx = 0;
        vertices[idx++] = (OGL_Vertex){_x - 0.5f, _y + 0.5f, _z + 0.5f, 0, 1, 0, 0, 0, texture};
        vertices[idx++] = (OGL_Vertex){_x + 0.5f, _y + 0.5f, _z + 0.5f, 0, 1, 0, 1, 0, texture};
        vertices[idx++] = (OGL_Vertex){_x + 0.5f, _y + 0.5f, _z - 0.5f, 0, 1, 0, 1, 1, texture};
        vertices[idx++] = (OGL_Vertex){_x + 0.5f, _y + 0.5f, _z - 0.5f, 0, 1, 0, 1, 1, texture};
        vertices[idx++] = (OGL_Vertex){_x - 0.5f, _y + 0.5f, _z - 0.5f, 0, 1, 0, 0, 1, texture};
        vertices[idx++] = (OGL_Vertex){_x - 0.5f, _y + 0.5f, _z + 0.5f, 0, 1, 0, 0, 0, texture};
    }
    void render()
    {
        glBindTexture(GL_TEXTURE_2D, texture);
        glColor3f((double)r / 255, (double)g / 255, (double)b / 255);

        glEnableClientState(GL_VERTEX_ARRAY);
        glVertexPointer(3, GL_FLOAT, sizeof(OGL_Vertex), &vertices[0].x);

        glEnableClientState(GL_TEXTURE_COORD_ARRAY);
        glTexCoordPointer(2, GL_FLOAT, sizeof(OGL_Vertex), &vertices[0].u);

        glDrawArrays(GL_TRIANGLES, 0, 6);

        glDisableClientState(GL_TEXTURE_COORD_ARRAY);
        glDisableClientState(GL_VERTEX_ARRAY);
    }
} OGL_Plane;

typedef struct OGL_Element : public Element, public OGL_Plane
{
    ColorRGB color;
    OGL_Element(Element element) : Element(element), OGL_Plane(0, 0, 0, 0, get_base()->color.r, get_base()->color.g, get_base()->color.b), color(get_base()->color)
    {
    }
    void set_position(float x, float y)
    {
        update_vertices(x, 0.1, y);
    }
} OGL_Element;

typedef struct OGL_Chunk
{
    OGL_Plane * tiles[CHUNK_SIZE * CHUNK_SIZE];
    std::unordered_map<size_t, OGL_Element *> elements;
    GLuint tiles_display_list = 0;
    GLuint element_display_list = 0;

    void render()
    {
        if (tiles_display_list != 0)
            glCallList(tiles_display_list);
        if (element_display_list != 0)
            glCallList(element_display_list);
    }

    void add_element(OGL_Element * el)
    {
        elements[el->uid] = el;
        update_element_display_list();
    }

    void update_tiles_display_list()
    {
        if (tiles_display_list != 0)
            glDeleteLists(tiles_display_list, 1);
        tiles_display_list = glGenLists(1);
        glNewList(tiles_display_list, GL_COMPILE);
        for (int i = 0; i < CHUNK_SIZE * CHUNK_SIZE; i++)
        {
            tiles[i]->render();
        }
        glEndList();
    }

    void update_element_display_list()
    {
        if (element_display_list != 0)
            glDeleteLists(element_display_list, 0);
        element_display_list = glGenLists(1);
        glNewList(element_display_list, GL_COMPILE);
        for (auto [_, ogl_element] : elements)
            ogl_element->render();
        glEndList();
    }

    OGL_Chunk()
    {
    }
} OGL_Chunk;

typedef struct OGL_World
{
    OGL_Chunk * ogl_chunks[WORLD_SIZE][WORLD_SIZE];
    GLuint display_list = 0;

    void render(size_t from_x, size_t from_z, size_t to_x, size_t to_z)
    {
        for (int chi = from_x; chi <= to_x; chi++)
        {
            for (int chj = from_z; chj <= to_z; chj++)
            {
                if (OGL_Chunk * ch = ogl_chunks[chj][chi])
                {
                    ch->render();
                }
            }
        }
    }

    OGL_World()
    {
        for (int i = 0; i < WORLD_SIZE; i++)
        {
            for (int j = 0; j < WORLD_SIZE; j++)
            {
                ogl_chunks[j][i] = NULL;
            }
        }
    }
} OGL_World;

typedef struct OGL_Text
{
    GLuint texture;
    int char_width;
    int char_height;
    int chars_per_row;
    int first_char;
    int texture_width;
    int texture_height;

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
} OGL_Text;
