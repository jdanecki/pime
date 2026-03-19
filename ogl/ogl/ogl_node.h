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

  public:
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
};
