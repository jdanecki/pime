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
#include <list>
#include <unordered_map>
#include "ogl/ogl_node.h"
#include "ogl/ogl_plane.h"

class OGL_Chunk
{
    std::unordered_map<size_t, InventoryElement *> elements;

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

    void render(int cx, int cy)
    {
        for (auto [_, inv_element] : elements)
        {
            if (OGL_Node * node = dynamic_cast<OGL_Node *>(inv_element))
            {
                ItemLocation * loc = &inv_element->location;
                Dimensions * dim = &inv_element->dimensions;
                node->render(loc->get_world_x(), dim->height.value / 2, loc->get_world_y(), dim->width.value, dim->height.value, dim->length.value);
            }
        }
        for (int x = 0; x < CHUNK_SIZE; x++)
            for (int y = 0; y < CHUNK_SIZE; y++)
                tiles[y][x]->render(x + cx * CHUNK_SIZE, 0, y + cy * CHUNK_SIZE, 1, 1, 1);
    }
};
