#include "element_godot.h"
#include "godot_cpp/classes/box_mesh.hpp"
#include "godot_cpp/classes/material.hpp"
#include "godot_cpp/classes/mesh_instance3d.hpp"
#include "godot_cpp/classes/standard_material3d.hpp"
#include "godot_cpp/core/memory.hpp"
#include "godot_cpp/variant/color.hpp"
#include "godot_cpp/variant/vector3.hpp"

using namespace godot;

void ElementGodot::_bind_methods()
{
}

ElementGodot::ElementGodot() : element(0)
{
}

ElementGodot::~ElementGodot()
{
}

ElementGodot::ElementGodot(Element element) : element(element)
{}

void ElementGodot::_ready()
{
    Ref<StandardMaterial3D> mat = memnew(StandardMaterial3D);
    BaseElement* base = element.get_base();
    mat->set_albedo(Color(base->color.r / 255.0, base->color.g / 255.0, base->color.b /255.0, 1));
    Ref<BoxMesh> box = memnew(BoxMesh);
    box->set_size(Vector3(element.length.value / 32.0, element.height.value / 32.0, element.width.value/ 32.0));
    box->set_material(mat);
    mesh = memnew(MeshInstance3D);
    mesh->set_mesh(box);
    add_child(mesh);
}
