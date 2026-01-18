#ifndef PLAYER_GODOT_H
#define PLAYER_GODOT_H

#include "godot_cpp/classes/character_body3d.hpp"
#include <godot_cpp/classes/node3d.hpp>
#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/variant/utility_functions.hpp>
#include "../../core/player.h"
#include "godot_cpp/classes/mesh_instance3d.hpp"

namespace godot {

class PlayerGodot : public CharacterBody3D, public Player {
    GDCLASS(PlayerGodot, CharacterBody3D)
    MeshInstance3D* mesh;

public:
    PlayerGodot();
    PlayerGodot(Player player);
    ~PlayerGodot();

    void _ready() override;
    
protected:
    static void _bind_methods();

};
}

#endif
