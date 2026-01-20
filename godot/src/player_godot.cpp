#include "player_godot.h"
#include "godot_cpp/classes/capsule_mesh.hpp"
#include "godot_cpp/classes/mesh_instance3d.hpp"
#include "godot_cpp/classes/standard_material3d.hpp"
#include "godot_cpp/core/memory.hpp"

using namespace godot;

void PlayerGodot::_bind_methods()
{
}

PlayerGodot::PlayerGodot() : Player(0, SerializableCString("none"), ItemLocation::center(), 0, 0, 0)
{
}

PlayerGodot::PlayerGodot(Player player) : Player(player)
{
}

PlayerGodot::~PlayerGodot()
{
}

void PlayerGodot::_ready()
{
    Ref<StandardMaterial3D> mat = memnew(StandardMaterial3D);
    switch (clan.get()->id)
    {
        case Clan_Animal:
            mat->set_albedo(Color(0, 1, 0, 1));
            break;
        case Clan_Human:
            mat->set_albedo(Color(0.2, 0.2, 1, 1));
            break;
        case Clan_Dwarf:
            mat->set_albedo(Color(0.2, 0.1, 0.1, 1));
            break;
        case Clan_Elf:
            mat->set_albedo(Color(0.5, 0.9, 0.5, 1));
            break;
        case Clan_Niziolek:
            mat->set_albedo(Color(0.1, 0.9, 0.5, 1));
            break;
    }
    Ref<CapsuleMesh> m = memnew(CapsuleMesh);
    m->set_material(mat);
    mesh = memnew(MeshInstance3D);
    mesh->set_mesh(m);
    add_child(mesh);
}
