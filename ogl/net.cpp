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
#include "../net/net.h"
#include "../core/chunk.h"
#include "main.h"
#include "ogl.h"
#include <cstdio>
#include <cstdlib>
#include <string.h>

void got_id(size_t id, int64_t seed)
{
    printf("GOT ID: %zu\n", id);
    my_id = id;
    srand(seed);
}

void update_chunk(int32_t x, int32_t y, const chunk_table * data)
{
    data = (chunk_table *)((char *)(data));
    if (!world_table[y][x])
    {
        world_table[y][x] = new chunk(x, y);
        memcpy(world_table[y][x]->table, &data[0], CHUNK_SIZE * CHUNK_SIZE * sizeof(int));
        ogl_world->ogl_chunks[y][x] = new OGL_Chunk();
        chunk * ch = world_table[y][x];
        OGL_Chunk * ogl_ch = ogl_world->ogl_chunks[y][x];
        for (int i = 0; i < CHUNK_SIZE; i++)
        {
            for (int j = 0; j < CHUNK_SIZE; j++)
            {
                BaseElement * be = get_base_element(ch->table[j][i].tile);
                ogl_ch->tiles[i * CHUNK_SIZE + j] = new OGL_Plane(OGL_Position(i + x * CHUNK_SIZE, 0, j + y * CHUNK_SIZE), OGL_Color(be->color.r, be->color.g, be->color.b), ch->table[j][i].tile);
            }
        }
        ogl_ch->update_tiles_display_list();
    }
}

void update_object(const ObjectData * data)
{
}

void update_item_location(LocationUpdateData data)
{
    printf("update_item_location\n");
    if (data.id.uid == my_id)
    {
        cam.x = data.new_.get_world_x();
        cam.z = data.new_.get_world_y();
    }

    OGL_Chunk * old_ch = ogl_world->ogl_chunks[data.old.chunk.map_y][data.old.chunk.map_x];
    InventoryElement * el = get_object_by_id(data.id);

    if (!el)
    {
        printf("el is null in update_item_location\n");
        return;
    }

    if (data.old.tag == ItemLocation::Tag::Chunk)
        old_ch->remove_element(data.id.get_uid());

    if (data.new_.tag == ItemLocation::Tag::Player)
    {
        printf("UNIMPLEMENTED! ItemLocation::Tag::Player\n");
        return;
    }

    OGL_Chunk * new_ch = ogl_world->ogl_chunks[data.new_.chunk.map_y][data.new_.chunk.map_x];
    if (!new_ch)
    {
        printf("update_item_location new_ch is null!\n");
        return;
    }

    if (OGL_Element * oel = dynamic_cast<OGL_Element *>(el))
    {
        printf("The new get_world_x is %f\n", data.new_.get_world_x());
        printf("The new get_world_y is %f\n", data.new_.get_world_y());
        oel->ogl_position = OGL_Position(data.new_.get_world_x(), oel->ogl_dimensions.height / 2, data.new_.get_world_y());
        oel->update_vertices();
    }
    if (OGL_Player * pl = dynamic_cast<OGL_Player *>(el))
    {
        pl->ogl_position = OGL_Position(data.new_.get_world_x(), 1, data.new_.get_world_y());
        pl->update_vertices();
    }
    el->location.chunk = data.new_.chunk;

    printf("Adding element to chunk\n");
    new_ch->add_element(el);
}

void create_object(const ObjectData * data_const)
{
    ObjectData * data = (ObjectData *)data_const;
    switch (data->tag)
    {
        case ObjectData::Tag::Element:
        {
            OGL_Element * el = new OGL_Element(data->element.data);
            el->set_position(el->location.get_world_x(), el->location.get_world_y());
            register_object(el);
            if (OGL_Chunk * ch = ogl_world->ogl_chunks[el->location.chunk.map_y][el->location.chunk.map_x])
            {
                ch->add_element(el);
            }
            break;
        }
        case ObjectData::Tag::Player:
        {
            OGL_Player * el = new OGL_Player(data->player.data);
            el->set_position(el->location.get_world_x(), el->location.get_world_y());
            register_object(el);
            if (OGL_Chunk * ch = ogl_world->ogl_chunks[el->location.chunk.map_y][el->location.chunk.map_x])
            {
                ch->add_element(el);
            }
            break;
        }
    }
}

void destroy_object(NetworkObject id, ItemLocation location)
{
}
void failed_craft()
{
}
void action_failed()
{
}
void knowledge_update(size_t pl_id, Class_id cid, int32_t id)
{
}
void checked_update(size_t pl_id, uintptr_t el)
{
}
