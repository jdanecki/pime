#include "../net/net.h"
#include "../core/tiles.h"
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
                ogl_ch->tiles[i * CHUNK_SIZE + j] = new OGL_Plane(i + x * CHUNK_SIZE, 0, j + y * CHUNK_SIZE, ch->table[j][i].tile % 15 + 1, be->color.r, be->color.g, be->color.b);
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
    if (data.id.uid == my_id)
    {
        cam.x = data.new_.get_world_x();
        cam.z = data.new_.get_world_y();
    }
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
