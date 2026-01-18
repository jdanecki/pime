#ifndef ELEMENT_GODOT_H
#define ELEMENT_GODOT_H

#include "godot_cpp/classes/mesh_instance3d.hpp"
#include <godot_cpp/classes/node3d.hpp>
#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/variant/utility_functions.hpp>
#include "../../core/alchemist/elements.h"

namespace godot
{

class ElementGodot : public Node3D, public Element
{
    GDCLASS(ElementGodot, Node3D)
    MeshInstance3D* mesh;

  public:
    ElementGodot();
    ~ElementGodot();
    ElementGodot(Element element);
    void _ready() override;

  protected:
    static void _bind_methods();
};
} // namespace godot

#endif
