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
#include <vector>
#include <SDL3/SDL.h>
#include "ogl_shaders.h"
#include "ogl_loader.h"
#include "ogl_vertex.h"

class OGL_Text
{
    static OGL_Text * instance;

    GLuint texture;
    GLuint vao = 0;
    GLuint vbo = 0;
    GLuint ebo = 0;

    int char_width;
    int char_height;
    int chars_per_row;
    int first_char;
    int texture_width;
    int texture_height;
    void push_char(char c, float x, float y, float scale, std::vector<OGL_Vertex> & verts, std::vector<GLushort> & idx)
    {
        int ci = (unsigned char)c - first_char;
        if (ci < 0)
            return;

        int col = ci % chars_per_row;
        int row = ci / chars_per_row;

        float u1 = (float)(col * char_width) / texture_width;
        float v1 = (float)(row * char_height) / texture_height;
        float u2 = (float)((col + 1) * char_width) / texture_width;
        float v2 = (float)((row + 1) * char_height) / texture_height;

        float w = char_width * scale;
        float h = char_height * scale;

        GLushort base = (GLushort)verts.size();
        verts.push_back({x, y, 0.f, 0.f, 0.f, 1.f, u1, v1, texture});
        verts.push_back({x + w, y, 0.f, 0.f, 0.f, 1.f, u2, v1, texture});
        verts.push_back({x + w, y + h, 0.f, 0.f, 0.f, 1.f, u2, v2, texture});
        verts.push_back({x, y + h, 0.f, 0.f, 0.f, 1.f, u1, v2, texture});

        idx.insert(idx.end(), {base, (GLushort)(base + 1), (GLushort)(base + 2), base, (GLushort)(base + 2), (GLushort)(base + 3)});
    }

    static void make_ortho(float out[16], float l, float r, float b, float t)
    {
        memset(out, 0, 64);
        out[0] = 2.f / (r - l);
        out[5] = 2.f / (t - b);
        out[10] = -1.f;
        out[12] = -(r + l) / (r - l);
        out[13] = -(t + b) / (t - b);
        out[15] = 1.f;
    }

    OGL_Text(const char * image_path, int char_width, int char_height, int chars_per_row, int first_char)
        : char_width(char_width), char_height(char_height), chars_per_row(chars_per_row), first_char(first_char)
    {
        OGL_Loader * gl = OGL_Loader::get_instance();

        SDL_Surface * surf = SDL_LoadPNG(image_path);
        SDL_assert(surf && "couldn't open font file");

        SDL_Surface * rgba = SDL_ConvertSurface(surf, SDL_PIXELFORMAT_RGBA32);
        SDL_DestroySurface(surf);
        SDL_assert(rgba);

        glGenTextures(1, &texture);
        glBindTexture(GL_TEXTURE_2D, texture);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, rgba->w, rgba->h, 0, GL_RGBA, GL_UNSIGNED_BYTE, rgba->pixels);
        texture_width = rgba->w;
        texture_height = rgba->h;
        SDL_DestroySurface(rgba);

        gl->glGenVertexArrays(1, &vao);
        gl->glGenBuffers(1, &vbo);
        gl->glGenBuffers(1, &ebo);

        gl->glBindVertexArray(vao);
        gl->glBindBuffer(GL_ARRAY_BUFFER, vbo);
        gl->glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);

        gl->glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(OGL_Vertex), (void *)offsetof(OGL_Vertex, x));
        gl->glEnableVertexAttribArray(0);

        gl->glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(OGL_Vertex), (void *)offsetof(OGL_Vertex, nx));
        gl->glEnableVertexAttribArray(1);

        gl->glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(OGL_Vertex), (void *)offsetof(OGL_Vertex, u));
        gl->glEnableVertexAttribArray(2);

        gl->glVertexAttribIPointer(3, 1, GL_UNSIGNED_INT, sizeof(OGL_Vertex), (void *)offsetof(OGL_Vertex, tex_id));
        gl->glEnableVertexAttribArray(3);

        gl->glBindVertexArray(0);
    }

  public:
    void draw_text(const char * text, float x, float y, float scale, int screen_width, int screen_height, float r = 1.f, float g = 1.f, float b = 1.f, float a = 1.f)
    {
        OGL_Loader * gl = OGL_Loader::get_instance();
        OGL_Shaders * sh = OGL_Shaders::get_instance();

        std::vector<OGL_Vertex> verts;
        std::vector<GLushort> indices;
        verts.reserve(128 * 4);
        indices.reserve(128 * 6);

        float cx = x, cy = y;
        for (int i = 0; text[i]; ++i)
        {
            if (text[i] == '\n')
            {
                cx = x;
                cy += char_height * scale;
                continue;
            }
            push_char(text[i], cx, cy, scale, verts, indices);
            cx += char_width * scale;
        }
        if (indices.empty())
            return;

        gl->glBindVertexArray(vao);
        gl->glBindBuffer(GL_ARRAY_BUFFER, vbo);
        gl->glBufferData(GL_ARRAY_BUFFER, verts.size() * sizeof(OGL_Vertex), verts.data(), GL_DYNAMIC_DRAW);
        gl->glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
        gl->glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(GLushort), indices.data(), GL_DYNAMIC_DRAW);

        GLboolean depth_was = glIsEnabled(GL_DEPTH_TEST);
        GLboolean cull_was = glIsEnabled(GL_CULL_FACE);
        GLboolean blend_was = glIsEnabled(GL_BLEND);
        GLint prev_program;
        glGetIntegerv(GL_CURRENT_PROGRAM, &prev_program);

        glDisable(GL_DEPTH_TEST);
        glDisable(GL_CULL_FACE);
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        gl->glUseProgram(sh->get_program_2d());

        float proj[16];
        make_ortho(proj, 0.f, (float)screen_width, (float)screen_height, 0.f);
        gl->glUniformMatrix4fv(sh->get_proj_location_2d(), 1, GL_FALSE, proj);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, texture);
        gl->glUniform1i(sh->get_tex_location_2d(), 0);
        gl->glUniform4f(sh->get_color_location_2d(), r, g, b, a);

        glDrawElements(GL_TRIANGLES, (GLsizei)indices.size(), GL_UNSIGNED_SHORT, nullptr);

        gl->glUseProgram(prev_program);
        gl->glBindVertexArray(0);
        if (!blend_was)
            glDisable(GL_BLEND);
        if (depth_was)
            glEnable(GL_DEPTH_TEST);
        if (cull_was)
            glEnable(GL_CULL_FACE);
        gl->glBindBuffer(GL_ARRAY_BUFFER, 0);
        gl->glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
        glBindTexture(GL_TEXTURE_2D, 0);
        glActiveTexture(GL_TEXTURE0);
    }

    static OGL_Text * get_instance()
    {
        if (!instance)
            instance = new OGL_Text("font.png", 8, 12, 16, 32);
        return instance;
    }
};
