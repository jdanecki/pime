#ifndef CHUNK_RENDERER_H
#define CHUNK_RENDERER_H

#include <godot_cpp/classes/node3d.hpp>
#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/variant/utility_functions.hpp>

#include "../../core/tiles.h"
#include "godot_cpp/classes/mesh_instance3d.hpp"

namespace godot
{

class ChunkRenderer : public Node3D
{
    GDCLASS(ChunkRenderer, Node3D)

  public:
    ChunkRenderer();
    ~ChunkRenderer();

    void update(const chunk_table * data);

  protected:
    static void _bind_methods();
    MeshInstance3D * tiles[CHUNK_SIZE][CHUNK_SIZE];
};
} // namespace godot

#endif
