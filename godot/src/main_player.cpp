#include "main_player.h"
#include <godot_cpp/classes/input_event_mouse_motion.hpp>
#include "godot_cpp/classes/capsule_mesh.hpp"
#include "godot_cpp/classes/camera3d.hpp"
#include "godot_cpp/classes/input.hpp"
#include "godot_cpp/classes/mesh.hpp"
#include "godot_cpp/classes/world3d.hpp"
#include "godot_cpp/classes/engine.hpp"
#include "godot_cpp/classes/viewport.hpp"
#include "godot_cpp/classes/physics_ray_query_parameters3d.hpp"
#include "godot_cpp/classes/physics_direct_space_state3d.hpp"
#include "godot_cpp/classes/standard_material3d.hpp"
#include "godot_cpp/variant/color.hpp"
#include "../../client-common/net.h"
#include "godot_cpp/variant/utility_functions.hpp"
#include "godot_cpp/variant/vector2.hpp"
#include "godot_cpp/variant/vector3.hpp"

using namespace godot;

void MainPlayer::_bind_methods() {

}

MainPlayer::MainPlayer() {
}

MainPlayer::MainPlayer(Player player) : PlayerGodot(player) {
    total_pitch = 0;
    mouse_sensitivity = 1.4;
    looking_at = nullptr;
}

MainPlayer::~MainPlayer() {
}

void MainPlayer::_ready() {
    PlayerGodot::_ready();
    Object::cast_to<StandardMaterial3D>(*Object::cast_to<CapsuleMesh>(*mesh->get_mesh())->get_material())->set_albedo(Color(1, 1, 1, 1));
    set_name("MainPlayer");

    Camera3D* camera = memnew(Camera3D);
    camera->set_name("Camera3D");
    add_child(camera);
    camera->set_position(Vector3(0, 0.8, 0));
}

void MainPlayer::_process(double delta) {
    Input* input = Input::get_singleton();

    Vector2 move_input = input->get_vector("move_left", "move_right", "move_forward", "move_back");
    Vector3 move_direction = Vector3(move_input.x, 0, move_input.y).rotated(Vector3(0, 1, 0), get_rotation().y) * 0.2;
    send_packet_move(move_direction.x, move_direction.z);
}

void MainPlayer::_input(const Ref<InputEvent> &event) {
    if (event->is_action_pressed("interact")) {
        if (looking_at) {
            send_packet_pickup(looking_at->uid);
        }
    }
    if (event->is_action_pressed("drop")) {
        if (inventory.head && inventory.head->get_el())
            send_packet_drop(inventory.head->get_el()->uid); 
    }
    Ref<InputEventMouseMotion> motion = event;
    if (motion.is_valid()) {
        float yaw = motion->get_relative().x * mouse_sensitivity;
        float pitch = motion->get_relative().y * mouse_sensitivity;
        pitch = Math::clamp(pitch, -90 - total_pitch, 90-total_pitch);
        total_pitch += pitch;

        rotate_y(Math::deg_to_rad(-yaw));
        get_node<Camera3D>("Camera3D")->rotate_object_local(Vector3(1, 0, 0), Math::deg_to_rad(-pitch));
        //rotate_object_local(Vector3(1, 0, 0), Math::deg_to_rad(-pitch));

    }
}

void MainPlayer::_physics_process(double delta) {
    if (Engine::get_singleton()->is_editor_hint())
        return;
    PhysicsDirectSpaceState3D* space_state = get_world_3d()->get_direct_space_state();
    Camera3D* camera = get_node<Camera3D>("Camera3D");
    
    Vector3 from = camera->project_ray_origin(get_viewport()->get_mouse_position());
    Vector3 to = from + camera->project_ray_normal(get_viewport()->get_mouse_position())* 100;
    Ref<PhysicsRayQueryParameters3D> parameters = PhysicsRayQueryParameters3D::create(from, to);
    TypedArray<RID> a;
    a.push_back(get_rid());
    parameters->set_exclude(a);
    Dictionary result = space_state->intersect_ray(parameters);
    looking_at = dynamic_cast<InventoryElement*>(Object::cast_to<Node3D>(result["collider"]));
    looking_pos = result["position"];
    // looking_norm = result["normal"];

}
