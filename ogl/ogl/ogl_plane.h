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
#include "ogl_node.h"

class OGL_Plane : public OGL_Node
{
    void update_vertices()
    {
        const float hw = 0.5;
        const float hd = 0.5;

        const float x = 0;
        const float y = 0;
        const float z = 0;

        int idx = 0;

        float r = (float)ogl_color.r / 255;
        float g = (float)ogl_color.g / 255;
        float b = (float)ogl_color.b / 255;
        vertices[idx++] = (OGL_Vertex){x - hw, y, z + hd, 0, 1, 0, 0, 0, texture, r, g, b};
        vertices[idx++] = (OGL_Vertex){x + hw, y, z + hd, 0, 1, 0, 1, 0, texture, r, g, b};
        vertices[idx++] = (OGL_Vertex){x + hw, y, z - hd, 0, 1, 0, 1, 1, texture, r, g, b};
        vertices[idx++] = (OGL_Vertex){x + hw, y, z - hd, 0, 1, 0, 1, 1, texture, r, g, b};
        vertices[idx++] = (OGL_Vertex){x - hw, y, z - hd, 0, 1, 0, 0, 1, texture, r, g, b};
        vertices[idx++] = (OGL_Vertex){x - hw, y, z + hd, 0, 1, 0, 0, 0, texture, r, g, b};
        // update_vbo_vao(); FIXME: use some 'managed' variable, rn only chunks use tiles
    }

  public:
    OGL_Plane(OGL_Color color, GLuint texture) : OGL_Node(color, texture, 6)
    {
        update_vertices();
    };
};
