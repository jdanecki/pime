#include "net_manager.h"

#include "chunk_renderer.h"
#include "element_godot.h"
#include "godot_cpp/classes/engine.hpp"
#include "godot_cpp/classes/node3d.hpp"
#include "godot_cpp/core/memory.hpp"
#include "godot_cpp/variant/string.hpp"
#include "godot_cpp/variant/utility_functions.hpp"
#include "godot_cpp/variant/variant.hpp"

using namespace godot;

NetManager* net_manager;

void NetManager::_bind_methods() {

}

NetManager::NetManager()
{
}

NetManager::~NetManager()
{
}

void NetManager::_ready()
{
    net_manager = this;
    if (!Engine::get_singleton()->is_editor_hint()) {
    client = init("127.0.0.1", "1234");
    if (client) {
        UtilityFunctions::print("Connected to server");
    } else {
        UtilityFunctions::print("Failed to connect");
    }
    }
}

void NetManager::_process(double delta)
{
    if (!Engine::get_singleton()->is_editor_hint())
    {
        network_tick(client);
    }
}

void NetManager::update_chunk(int x, int y, const chunk_table* data)
{
    String name = vformat("%d_%d", x, y);
    ChunkRenderer* chunk = Object::cast_to<ChunkRenderer>(get_node_or_null(name));
    if (!chunk) {
        chunk = memnew(ChunkRenderer);
        chunk->set_name(name);
        add_child(chunk);
    }
    chunk->update(data);
}

void NetManager::create_object(const ObjectData* data)
{
    switch (data->tag)
    {
        case ObjectData::Tag::Element: 
            ElementGodot* el = memnew(ElementGodot(data->element.data));
            add_child(el);
            el->set_position(Vector3(data->element.data.location.chunk.x, 1, data->element.data.location.chunk.y ));
    }
}

size_t my_id;

// FIXME remove when net.cpp is cleaned up
void update_hotbar() {}

void print_status(int l, const char * format, ...)
{
    va_list args;
    va_start(args, format);
    UtilityFunctions::print(format, args);
    va_end(args);
}

int CONSOLE_LOG(const char * fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    UtilityFunctions::print(fmt, args);
    va_end(args);
    return 0;
}

void update_chunk(int32_t x, int32_t y, const chunk_table * data)
{
    net_manager->update_chunk(x, y, data);
}

void got_id(size_t id, int64_t seed)
{
    my_id = id;
}

void update_object(const ObjectData * data)
{
}

void update_item_location(LocationUpdateData data)
{
}

void create_object(const ObjectData * data)
{
    net_manager->create_object(data);
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
