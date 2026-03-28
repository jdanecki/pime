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
    GLuint texture;
    int drawtype;
    OGL_Color ogl_color;

    GLuint vao = 0;
    GLuint vbo = 0;

  public:
    GLsizei vert_num;      // FIXME: should be protected
    OGL_Vertex * vertices; // FIXME: should be protected
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

        gl->glEnableVertexAttribArray(4);
        gl->glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, sizeof(OGL_Vertex), (void *)offsetof(OGL_Vertex, r));

        gl->glBindBuffer(GL_ARRAY_BUFFER, 0);
        gl->glBindVertexArray(0);
    }
    virtual void render(float x, float y, float z, float xs, float ys, float zs)
    {
        OGL_Loader * gl = OGL_Loader::get_instance();
        GLuint program = OGL_Shaders::get_instance()->get_program_3d();
        OGL_Shaders * sh = OGL_Shaders::get_instance();

        gl->glUniform3f(sh->get_model_transform_location_3d(), x, y, z);
        gl->glUniform3f(sh->get_scale_location_3d(), xs, ys, zs);
        gl->glUniform4f(sh->get_color_location_3d(), (float)ogl_color.r / 255.0f, (float)ogl_color.g / 255.0f, (float)ogl_color.b / 255.0f, 1.0); // FIXME: support transparency
                                                                                                                                                  // FIXME: use core variables once implemented
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, texture);
        gl->glUniform1i(sh->get_tex_location_3d(), 0);

        gl->glBindVertexArray(vao);
        glDrawArrays(drawtype, 0, vert_num);
        gl->glBindVertexArray(0);

        glBindTexture(GL_TEXTURE_2D, 0);
    }
};
