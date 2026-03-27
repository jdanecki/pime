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
#include "../core/alchemist/elements/plant.h"

class OGL_Plant : public Plant, public OGL_Node
{
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

    void render(float x, float y, float z, float xs, float ys, float zs) override
    {
        OGL_Node::render(x, 0, z, xs, ys, zs);
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
        // if (rand() % 4)
        // generative_iter(v, v[(*i)], i, line_vert_num, size_halfed);
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
        update_vbo_vao();
    }

  public:
    OGL_Plant(Plant plant) : Plant(plant), OGL_Node(OGL_Color(10, 200, 10), 0, 100)
    {
        update_vertices();
        drawtype = GL_LINES;
    }
};
