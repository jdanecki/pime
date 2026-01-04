#ifndef ELEMENT_GODOT_H
#define ELEMENT_GODOT_H

#include <godot_cpp/classes/node3d.hpp>
#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/variant/utility_functions.hpp>

namespace godot {

class ElementGodot : public Node3D {
    GDCLASS(ElementGodot, Node3D)

public:
    ElementGodot();
    ~ElementGodot();
    
protected:
    static void _bind_methods();

};
}

#endif
