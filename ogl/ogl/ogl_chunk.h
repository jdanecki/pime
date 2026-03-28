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
#include "../core/alchemist/elements/inventory_element.h"
#include "../core/world_params.h"
#include <SDL3/SDL.h>
#include <cstdio>
#include <list>
#include <unordered_map>
#include "ogl/ogl_node.h"
#include "ogl/ogl_plane.h"
#include "ogl/ogl_shaders.h"

extern GLuint texture_array_id;

class OGL_Chunk
{
    std::unordered_map<size_t, InventoryElement *> elements;
    bool dirty = true;
    GLuint vbo = 0;
    GLuint vao = 0;

  public:
    void add_element(InventoryElement * el)
    {
        elements[el->uid] = el;
    }

    void remove_element(size_t uid)
    {
        elements.erase(uid);
    }

    OGL_Plane * tiles[CHUNK_SIZE][CHUNK_SIZE];

    OGL_Chunk()
    {
    }

    std::list<InventoryElement *> get_elements_from_position(float x, float y, float z)
    {
        std::list<InventoryElement *> retval;
        for (auto [_, inv_element] : elements)
        {
            SDL_assert(inv_element != NULL);
            if (abs(x - inv_element->location.get_world_x()) < inv_element->dimensions.width.value / 2)
                if (abs(z - inv_element->location.get_world_y()) < inv_element->dimensions.length.value / 2)
                {
                    if (abs(y) < inv_element->dimensions.height.value) // FIXME: once 3D is added to core
                    {
                        retval.push_back(inv_element);
                    }
                }
        }
        return retval;
    }

    void update_vbo_vao(int cx, int cy)
    {
        OGL_Loader * gl = OGL_Loader::get_instance();

        int total_verts = CHUNK_SIZE * CHUNK_SIZE * 6;
        OGL_Vertex * buf = new OGL_Vertex[total_verts];
        int idx = 0;

        for (int x = 0; x < CHUNK_SIZE; x++)
            for (int y = 0; y < CHUNK_SIZE; y++)
            {
                OGL_Plane * tile = tiles[y][x];
                for (int v = 0; v < tile->vert_num; v++)
                {
                    buf[idx] = tile->vertices[v];
                    buf[idx].x += x + cx * CHUNK_SIZE;
                    buf[idx].z += y + cy * CHUNK_SIZE;
                    idx++;
                }
            }

        if (!vao)
            gl->glGenVertexArrays(1, &vao);
        if (!vbo)
            gl->glGenBuffers(1, &vbo);

        gl->glBindVertexArray(vao);
        gl->glBindBuffer(GL_ARRAY_BUFFER, vbo);
        gl->glBufferData(GL_ARRAY_BUFFER, sizeof(OGL_Vertex) * total_verts, buf, GL_STATIC_DRAW);

        gl->glEnableVertexAttribArray(0);
        gl->glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(OGL_Vertex), (void *)offsetof(OGL_Vertex, x));
        gl->glEnableVertexAttribArray(1);
        gl->glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(OGL_Vertex), (void *)offsetof(OGL_Vertex, nx));
        gl->glEnableVertexAttribArray(2);
        gl->glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(OGL_Vertex), (void *)offsetof(OGL_Vertex, u));
        gl->glEnableVertexAttribArray(3);
        gl->glVertexAttribIPointer(3, 1, GL_INT, sizeof(OGL_Vertex), (void *)offsetof(OGL_Vertex, tex_id));
        gl->glEnableVertexAttribArray(4);
        gl->glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, sizeof(OGL_Vertex), (void *)offsetof(OGL_Vertex, r));

        gl->glBindBuffer(GL_ARRAY_BUFFER, 0);
        gl->glBindVertexArray(0);

        delete[] buf;
        dirty = false;
    }

    void render(int cx, int cy)
    {
        OGL_Loader * gl = OGL_Loader::get_instance();
        OGL_Shaders * sh = OGL_Shaders::get_instance();

        if (dirty)
            update_vbo_vao(cx, cy);

        gl->glUniform3f(sh->get_model_transform_location_3d(), 0, 0, 0);
        gl->glUniform3f(sh->get_scale_location_3d(), 1, 1, 1);
        gl->glUniform4f(sh->get_color_location_3d(), 1, 1, 1, 1);

        gl->glBindVertexArray(vao);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D_ARRAY, texture_array_id);
        glDrawArrays(GL_TRIANGLES, 0, CHUNK_SIZE * CHUNK_SIZE * 6);
        gl->glBindVertexArray(0);

        for (auto [_, inv_element] : elements)
        {
            if (OGL_Node * node = dynamic_cast<OGL_Node *>(inv_element))
            {
                ItemLocation * loc = &inv_element->location;
                Dimensions * dim = &inv_element->dimensions;
                node->render(loc->get_world_x(), dim->height.value / 2, loc->get_world_y(), dim->width.value, dim->height.value, dim->length.value);
            }
        }
    }
};
