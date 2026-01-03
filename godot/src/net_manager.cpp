#include "net_manager.h"

#include "../../SDL/networking.h"
#include "godot_cpp/classes/engine.hpp"
#include "godot_cpp/variant/utility_functions.hpp"

NetClient * client;

using namespace godot;

void NetManager::_bind_methods() {

}

NetManager::NetManager() {
}

NetManager::~NetManager() {
}

void NetManager::_ready()
{
    if (!Engine::get_singleton()->is_editor_hint()) {
    client = init("127.0.0.1", "1234");
    UtilityFunctions::print("Connected to server");
    }
}

void NetManager::_process(double delta)
{
    if (!Engine::get_singleton()->is_editor_hint()) {
    network_tick(client);
    }
}

void print_status(int l, const char * format, ...)
{
    va_list args;
    va_start(args, format);
    UtilityFunctions::print(format, args);
    va_end(args);
}

void update_player(uintptr_t id, int32_t map_x, int32_t map_y, int32_t x, int32_t y, int32_t thirst, int32_t hunger)
{
    
}

void update_chunk(int32_t x, int32_t y, const chunk_table * data)
{
    
}

void got_id(size_t id, int64_t seed)
{
    
}

void update_object(const ObjectData * data)
{
    
}

void update_item_location(LocationUpdateData data)
{
    
}

void create_object(const ObjectData * data)
{
    
}

void destroy_object(NetworkObject id, ItemLocation location)
{
    
}

void failed_craft()
{
    
}

void action_failed()
{
    
}

void knowledge_update(size_t pl_id, Class_id cid, int32_t id)
{
    
}

void checked_update(size_t pl_id, uintptr_t el)
{
    
}
