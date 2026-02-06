#include "chunk_renderer.h"
#include "godot_cpp/classes/material.hpp"
#include "godot_cpp/classes/mesh_instance3d.hpp"
#include "godot_cpp/classes/plane_mesh.hpp"
#include "godot_cpp/classes/standard_material3d.hpp"
#include "godot_cpp/core/memory.hpp"
#include "../../net/net.h"

using namespace godot;

void ChunkRenderer::_bind_methods()
{
}

ChunkRenderer::ChunkRenderer()
{
    for (int y = 0; y < CHUNK_SIZE; y++)
    {
        for (int x = 0; x < CHUNK_SIZE; x++)
        {
            tiles[y][x] = nullptr;
        }
    }
}

ChunkRenderer::~ChunkRenderer()
{
}

void ChunkRenderer::update(const chunk_table * data)
{
    for (int y = 0; y < CHUNK_SIZE; y++)
    {
        for (int x = 0; x < CHUNK_SIZE; x++)
        {
            if (tiles[y][x])
            {
                memfree(tiles[y][x]);
            }
            MeshInstance3D * mesh = memnew(MeshInstance3D);
            Ref<PlaneMesh> m = memnew(PlaneMesh);
            m->set_size(Vector2(1, 1));
            Ref<StandardMaterial3D> mat = memnew(StandardMaterial3D);
            BaseElement * el = get_base_element((*data)[y][x].tile);
            mat->set_albedo(Color(el->color.r / 255.0, el->color.g / 255.0, el->color.b / 255.0, 1));
            m->set_material(mat);
            mesh->set_mesh(m);
            tiles[y][x] = mesh;
            add_child(mesh);
            mesh->set_position(Vector3(x, 0, y));
        }
    }
}
