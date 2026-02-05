#include "main_player.h"
#include "godot_cpp/classes/capsule_mesh.hpp"
#include "godot_cpp/classes/input.hpp"
#include "godot_cpp/classes/mesh.hpp"
#include "godot_cpp/classes/standard_material3d.hpp"
#include "godot_cpp/variant/color.hpp"
#include "../../client-common/net.h"
#include "godot_cpp/variant/utility_functions.hpp"

using namespace godot;

void MainPlayer::_bind_methods() {

}

MainPlayer::MainPlayer() {
}

MainPlayer::MainPlayer(Player player) : PlayerGodot(player) {
}

MainPlayer::~MainPlayer() {
}

void MainPlayer::_ready() {
    PlayerGodot::_ready();
    Object::cast_to<StandardMaterial3D>(*Object::cast_to<CapsuleMesh>(*mesh->get_mesh())->get_material())->set_albedo(Color(1, 1, 1, 1));
}

void MainPlayer::_process(double delta) {
    UtilityFunctions::print(delta);
    Input* input = Input::get_singleton();
    if (input->is_action_pressed("move_left")) {
        send_packet_move(-0.2, 0);
    }
    if (input->is_action_pressed("move_right")) {
        send_packet_move(0.2, 0);
    }
    if (input->is_action_pressed("move_forward")) {
        send_packet_move(0, -0.2);
    }
    if (input->is_action_pressed("move_back")) {
        send_packet_move(0, 0.2);
    }
}
