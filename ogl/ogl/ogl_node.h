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
#include <cstddef>
#include "ogl/ogl_loader.h"
#include "ogl/ogl_shaders.h"
#include "ogl_color.h"
#include "ogl_vertex.h"

class OGL_Node
{
  protected:
    OGL_Vertex * vertices;
    GLuint texture;
    int drawtype;
    OGL_Color ogl_color;
    GLsizei vert_num;

    GLuint vao = 0;
    GLuint vbo = 0;

  public:
    OGL_Node(OGL_Color color, GLuint texture, GLsizei vert_num) : ogl_color(color), texture(texture), vert_num(vert_num), drawtype(GL_TRIANGLES)
    {
        vertices = new OGL_Vertex[vert_num];
    }
    void update_vbo_vao()
    {
        OGL_Loader * gl = OGL_Loader::get_instance();
        if (!vao)
            gl->glGenVertexArrays(1, &vao);
        if (!vbo)
            gl->glGenBuffers(1, &vbo);

        gl->glBindVertexArray(vao);
        gl->glBindBuffer(GL_ARRAY_BUFFER, vbo);
        gl->glBufferData(GL_ARRAY_BUFFER, sizeof(OGL_Vertex) * vert_num, vertices, GL_STATIC_DRAW);

        gl->glEnableVertexAttribArray(0);
        gl->glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(OGL_Vertex), (void *)offsetof(OGL_Vertex, x));

        gl->glEnableVertexAttribArray(1);
        gl->glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(OGL_Vertex), (void *)offsetof(OGL_Vertex, nx));

        gl->glEnableVertexAttribArray(2);
        gl->glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(OGL_Vertex), (void *)offsetof(OGL_Vertex, u));

        gl->glEnableVertexAttribArray(3);
        gl->glVertexAttribIPointer(3, 1, GL_UNSIGNED_INT, sizeof(OGL_Vertex), (void *)offsetof(OGL_Vertex, tex_id));

        gl->glBindBuffer(GL_ARRAY_BUFFER, 0);
        gl->glBindVertexArray(0);
    }
    virtual void render(float x, float y, float z, float xs, float ys, float zs)
    {
        OGL_Loader * gl = OGL_Loader::get_instance();
        GLuint program = OGL_Shaders::get_instance()->get_program_3d();

        GLint loc_transform = gl->glGetUniformLocation(program, "uModelTransform");
        gl->glUniform3f(loc_transform, x, y, z);

        GLint loc_scale = gl->glGetUniformLocation(program, "uScale");
        gl->glUniform3f(loc_scale, xs, ys, zs);

        GLint loc_color = gl->glGetUniformLocation(program, "uColor");
        gl->glUniform4f(loc_color, (float)ogl_color.r / 255.0f, (float)ogl_color.g / 255.0f, (float)ogl_color.b / 255.0f, 1.0); // FIXME: support transparency
                                                                                                                                // FIXME: use core variables once implemented

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, texture);

        GLint loc_tex = gl->glGetUniformLocation(program, "uTexture");
        gl->glUniform1i(loc_tex, 0);

        gl->glBindVertexArray(vao);
        glDrawArrays(drawtype, 0, vert_num);
        gl->glBindVertexArray(0);

        glBindTexture(GL_TEXTURE_2D, 0);
    }
};
