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

#include "ogl_chunk.h"

class OGL_World
{
    GLuint display_list = 0;

  public:
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

    OGL_Chunk * ogl_chunks[WORLD_SIZE][WORLD_SIZE];

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
};
