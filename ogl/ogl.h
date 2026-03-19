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
#include "../core/player.h"
#include "../core/alchemist/elements/plant.h"
#include <GL/gl.h>
#include <SDL3/SDL.h>
#include <cassert>
#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <unordered_map>

typedef struct OGL_Camera
{
    float x, y, z;
    float vy;
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

    void get_forward_vector(float * x, float * y, float * z)
    {
        float yaw_rad = -yaw * M_PI / 180.0f;
        float pitch_rad = -pitch * M_PI / 180.0f;
        *x = -cos(pitch_rad) * sin(yaw_rad);
        *y = -sin(pitch_rad);
        *z = -cos(pitch_rad) * cos(yaw_rad);
    }

    void rotate_by(float yaw, float pitch)
    {
        this->yaw += yaw;
        this->pitch += pitch;

        if (this->pitch > 90)
            this->pitch = 90;
        if (this->pitch < -90)
            this->pitch = -90;

        if (this->yaw < 0)
            this->yaw += 360;
        if (this->yaw >= 360)
            this->yaw -= 360;
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

typedef struct OGL_Color
{
    int r, g, b;
    OGL_Color(int r, int g, int b) : r(r), g(g), b(b) {};
} OGL_Color;

typedef struct OGL_Node
{
    GLuint texture;
    OGL_Color ogl_color;
    OGL_Vertex * vertices;
    GLsizei vert_num;
    int drawtype;
    OGL_Node(OGL_Color color, GLuint texture, GLsizei vert_num) : ogl_color(color), texture(texture), vert_num(vert_num), drawtype(GL_TRIANGLES)
    {
        vertices = new OGL_Vertex[vert_num];
    }
    void render(float x, float y, float z, float xs, float ys, float zs)
    {
        glPushMatrix();
        glTranslatef(x, y, z);
        glScalef(xs, ys, zs);
        glBindTexture(GL_TEXTURE_2D, texture);
        glColor3f((double)ogl_color.r / 255, (double)ogl_color.g / 255, (double)ogl_color.b / 255);

        glEnableClientState(GL_VERTEX_ARRAY);
        glVertexPointer(3, GL_FLOAT, sizeof(OGL_Vertex), &vertices[0].x);

        glEnableClientState(GL_TEXTURE_COORD_ARRAY);
        glTexCoordPointer(2, GL_FLOAT, sizeof(OGL_Vertex), &vertices[0].u);

        glDrawArrays(drawtype, 0, vert_num);

        glDisableClientState(GL_TEXTURE_COORD_ARRAY);
        glDisableClientState(GL_VERTEX_ARRAY);
        glPopMatrix();
    }
} OGL_Node;

typedef struct OGL_Plane : public OGL_Node
{
    OGL_Plane(OGL_Color color, GLuint texture) : OGL_Node(color, texture, 6)
    {
        update_vertices();
    };

    void update_vertices()
    {
        const float hw = 0.5;
        const float hd = 0.5;

        const float x = 0;
        const float y = 0;
        const float z = 0;

        int idx = 0;

        vertices[idx++] = (OGL_Vertex){x - hw, y, z + hd, 0, 1, 0, 0, 0, texture};
        vertices[idx++] = (OGL_Vertex){x + hw, y, z + hd, 0, 1, 0, 1, 0, texture};
        vertices[idx++] = (OGL_Vertex){x + hw, y, z - hd, 0, 1, 0, 1, 1, texture};
        vertices[idx++] = (OGL_Vertex){x + hw, y, z - hd, 0, 1, 0, 1, 1, texture};
        vertices[idx++] = (OGL_Vertex){x - hw, y, z - hd, 0, 1, 0, 0, 1, texture};
        vertices[idx++] = (OGL_Vertex){x - hw, y, z + hd, 0, 1, 0, 0, 0, texture};
    }
} OGL_Plane;

typedef struct OGL_Cube : public OGL_Node
{
    OGL_Cube(OGL_Color color, GLuint texture) : OGL_Node(color, texture, 36)
    {
        update_vertices();
    };
    void update_vertices()
    {
        const float hw = 0.5;
        const float hh = 0.5;
        const float hd = 0.5;

        const float x = 0;
        const float y = 0;
        const float z = 0;

        int idx = 0;

        vertices[idx++] = (OGL_Vertex){x + hw, y - hh, z + hd, 1, 0, 0, 0, 1, texture};
        vertices[idx++] = (OGL_Vertex){x + hw, y + hh, z - hd, 1, 0, 0, 1, 0, texture};
        vertices[idx++] = (OGL_Vertex){x + hw, y + hh, z + hd, 1, 0, 0, 0, 0, texture};
        vertices[idx++] = (OGL_Vertex){x + hw, y - hh, z + hd, 1, 0, 0, 0, 1, texture};
        vertices[idx++] = (OGL_Vertex){x + hw, y - hh, z - hd, 1, 0, 0, 1, 1, texture};
        vertices[idx++] = (OGL_Vertex){x + hw, y + hh, z - hd, 1, 0, 0, 1, 0, texture};

        vertices[idx++] = (OGL_Vertex){x - hw, y - hh, z - hd, -1, 0, 0, 0, 1, texture};
        vertices[idx++] = (OGL_Vertex){x - hw, y + hh, z + hd, -1, 0, 0, 1, 0, texture};
        vertices[idx++] = (OGL_Vertex){x - hw, y + hh, z - hd, -1, 0, 0, 0, 0, texture};
        vertices[idx++] = (OGL_Vertex){x - hw, y - hh, z - hd, -1, 0, 0, 0, 1, texture};
        vertices[idx++] = (OGL_Vertex){x - hw, y - hh, z + hd, -1, 0, 0, 1, 1, texture};
        vertices[idx++] = (OGL_Vertex){x - hw, y + hh, z + hd, -1, 0, 0, 1, 0, texture};

        vertices[idx++] = (OGL_Vertex){x - hw, y + hh, z + hd, 0, 1, 0, 0, 1, texture};
        vertices[idx++] = (OGL_Vertex){x + hw, y + hh, z + hd, 0, 1, 0, 1, 1, texture};
        vertices[idx++] = (OGL_Vertex){x + hw, y + hh, z - hd, 0, 1, 0, 1, 0, texture};
        vertices[idx++] = (OGL_Vertex){x - hw, y + hh, z + hd, 0, 1, 0, 0, 1, texture};
        vertices[idx++] = (OGL_Vertex){x + hw, y + hh, z - hd, 0, 1, 0, 1, 0, texture};
        vertices[idx++] = (OGL_Vertex){x - hw, y + hh, z - hd, 0, 1, 0, 0, 0, texture};

        vertices[idx++] = (OGL_Vertex){x - hw, y - hh, z - hd, 0, -1, 0, 0, 1, texture};
        vertices[idx++] = (OGL_Vertex){x + hw, y - hh, z - hd, 0, -1, 0, 1, 1, texture};
        vertices[idx++] = (OGL_Vertex){x + hw, y - hh, z + hd, 0, -1, 0, 1, 0, texture};
        vertices[idx++] = (OGL_Vertex){x - hw, y - hh, z - hd, 0, -1, 0, 0, 1, texture};
        vertices[idx++] = (OGL_Vertex){x + hw, y - hh, z + hd, 0, -1, 0, 1, 0, texture};
        vertices[idx++] = (OGL_Vertex){x - hw, y - hh, z + hd, 0, -1, 0, 0, 0, texture};

        vertices[idx++] = (OGL_Vertex){x - hw, y - hh, z + hd, 0, 0, 1, 0, 1, texture};
        vertices[idx++] = (OGL_Vertex){x + hw, y - hh, z + hd, 0, 0, 1, 1, 1, texture};
        vertices[idx++] = (OGL_Vertex){x + hw, y + hh, z + hd, 0, 0, 1, 1, 0, texture};
        vertices[idx++] = (OGL_Vertex){x - hw, y - hh, z + hd, 0, 0, 1, 0, 1, texture};
        vertices[idx++] = (OGL_Vertex){x + hw, y + hh, z + hd, 0, 0, 1, 1, 0, texture};
        vertices[idx++] = (OGL_Vertex){x - hw, y + hh, z + hd, 0, 0, 1, 0, 0, texture};

        vertices[idx++] = (OGL_Vertex){x + hw, y - hh, z - hd, 0, 0, -1, 0, 1, texture};
        vertices[idx++] = (OGL_Vertex){x - hw, y - hh, z - hd, 0, 0, -1, 1, 1, texture};
        vertices[idx++] = (OGL_Vertex){x - hw, y + hh, z - hd, 0, 0, -1, 1, 0, texture};
        vertices[idx++] = (OGL_Vertex){x + hw, y - hh, z - hd, 0, 0, -1, 0, 1, texture};
        vertices[idx++] = (OGL_Vertex){x - hw, y + hh, z - hd, 0, 0, -1, 1, 0, texture};
        vertices[idx++] = (OGL_Vertex){x + hw, y + hh, z - hd, 0, 0, -1, 0, 0, texture};
    }
} OGL_Cube;

typedef struct OGL_Element : public Element, public OGL_Cube
{
    OGL_Element(Element element) : Element(element), OGL_Cube(OGL_Color(get_base()->color.r, get_base()->color.g, get_base()->color.b), 0)
    {
    }
} OGL_Element;

typedef struct OGL_Plant : public Plant, public OGL_Node
{
    OGL_Plant(Plant plant) : Plant(plant), OGL_Node(OGL_Color(10, 200, 10), 0, 100)
    {
        update_vertices();
        drawtype = GL_LINES;
    }

    OGL_Vertex return_modified(OGL_Vertex * original, OGL_Vertex * size_halfed, int i, int line_vert_num)
    {
        float max_x_size = size_halfed->x;
        float max_y_size = size_halfed->y * 2;
        float max_z_size = size_halfed->z;
        float mod_x = ((float)(rand() % 8) - 4) / 4 * max_z_size;
        float mod_y = ((float)(rand() % 8) / 8) * max_y_size;
        float mod_z = ((float)(rand() % 8) - 4) / 4 * max_x_size;
        return {original->x + mod_x, original->y + mod_y, original->z + mod_z};
    }

    void generative_iter(OGL_Vertex * v, OGL_Vertex v_in, int * i, int line_vert_num, OGL_Vertex * size_halfed)
    {
        if (*i >= line_vert_num)
            return;

        if (*i != 0)
        {
            (*i)++;
            if (*i >= line_vert_num)
                return;
            v[*i] = v_in;
        }
        (*i)++;
        if (*i >= line_vert_num)
            return;
        v[*i] = return_modified(&v_in, size_halfed, *i, line_vert_num);

        if (rand() % 4)
            generative_iter(v, v[(*i) - 1], i, line_vert_num, size_halfed);
        if (rand() % 4)
            generative_iter(v, v[(*i) - 1], i, line_vert_num, size_halfed);
        if (rand() % 4)
            generative_iter(v, v[(*i) - 2], i, line_vert_num, size_halfed);
        if (rand() % 4)
            generative_iter(v, v[(*i)], i, line_vert_num, size_halfed);
        if (rand() % 4)
            generative_iter(v, v[(*i) - 2], i, line_vert_num, size_halfed);
    }

    void update_vertices()
    {
        const float hw = 0.5f;
        const float hh = 0.5f;
        const float hd = 0.5f;
        int i = 0;
        OGL_Vertex size_halfed = (OGL_Vertex){hw, hh, hd, 0, 0, 0, 0, 0, 0};
        vertices[0] = (OGL_Vertex){0, 0, 0, 0, 0, 0, 0, 0, 0};
        generative_iter(vertices, vertices[0], &i, vert_num, &size_halfed);
    }
} OGL_Plant;

typedef struct OGL_Player : public Player, public OGL_Cube
{
    OGL_Player(Player player) : Player(player), OGL_Cube(OGL_Color(255, 255, 255), 0)
    {
    }
} OGL_Player;

typedef struct OGL_Chunk
{
    OGL_Plane * tiles[CHUNK_SIZE][CHUNK_SIZE];
    std::unordered_map<size_t, InventoryElement *> elements;
    GLuint tiles_display_list = 0;
    GLuint element_display_list = 0;

    void render()
    {
        if (tiles_display_list != 0)
            glCallList(tiles_display_list);
        if (element_display_list != 0)
            glCallList(element_display_list);
    }

    void add_element(InventoryElement * el)
    {
        printf("Adding element %zu\n", el->uid);
        elements[el->uid] = el;
        update_element_display_list();
    }

    void remove_element(size_t uid)
    {
        printf("Removing element %zu\n", uid);
        elements.erase(uid);
        update_element_display_list();
    }

    void update_tiles_display_list()
    {
        printf("update_tiles_display_list called\n");
        if (tiles_display_list != 0)
            glDeleteLists(tiles_display_list, 1);
        tiles_display_list = glGenLists(1);
        glNewList(tiles_display_list, GL_COMPILE);
        for (int x = 0; x < CHUNK_SIZE; x++)
            for (int y = 0; y < CHUNK_SIZE; y++)
                tiles[y][x]->render(x, 0, y, 1, 1, 1);
        glEndList();
    }

    void update_element_display_list()
    {
        printf("update_element_display_list called\n");
        if (element_display_list != 0)
            glDeleteLists(element_display_list, 0);
        element_display_list = glGenLists(1);
        glNewList(element_display_list, GL_COMPILE);
        for (auto [_, inv_element] : elements)
        {
            if (OGL_Node * node = dynamic_cast<OGL_Node *>(inv_element))
            {
                ItemLocation * loc = &inv_element->location;
                Dimensions * dim = &inv_element->dimensions;
                node->render(loc->get_tile_x(), dim->height.value / 2, loc->get_tile_y(), dim->width.value, dim->height.value, dim->length.value);
            }
        }
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
                    glPushMatrix();
                    glTranslatef(chi * CHUNK_SIZE, 0, chj * CHUNK_SIZE);
                    ch->render();
                    glPopMatrix();
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
