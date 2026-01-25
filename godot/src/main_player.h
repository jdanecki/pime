#ifndef MAIN_PLAYER_H
#define MAIN_PLAYER_H

#include <godot_cpp/classes/node3d.hpp>
#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/variant/utility_functions.hpp>
#include <godot_cpp/classes/input_event.hpp>
#include "godot_cpp/variant/vector3.hpp"
#include "player_godot.h"

namespace godot {

class MainPlayer : public PlayerGodot {
    GDCLASS(MainPlayer, PlayerGodot)

public:
    MainPlayer();
    MainPlayer(Player player);
    ~MainPlayer();

    void _ready() override;
    void _process(double delta) override;
    void _input(const Ref<InputEvent> &event) override;
    void _physics_process(double delta);
    
protected:
    static void _bind_methods();

private:
    float total_pitch;
    float mouse_sensitivity;
    InventoryElement* looking_at;
    Vector3 looking_pos;

};
}

#endif
