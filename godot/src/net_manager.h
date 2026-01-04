#ifndef NET_MANAGER_H
#define NET_MANAGER_H

#include <godot_cpp/classes/node3d.hpp>
#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/variant/utility_functions.hpp>

namespace godot {

class NetManager : public Node3D {
    GDCLASS(NetManager, Node3D)

public:
    NetManager();
    ~NetManager();
    void _ready() override;
    void _process(double delta) override;
    
protected:
    static void _bind_methods();

};
}

#endif
