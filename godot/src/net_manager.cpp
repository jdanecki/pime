#include "net_manager.h"

#include "chunk_renderer.h"
#include "element_godot.h"
#include "godot_cpp/classes/engine.hpp"
#include "godot_cpp/classes/node3d.hpp"
#include "godot_cpp/core/memory.hpp"
#include "godot_cpp/variant/string.hpp"
#include "godot_cpp/variant/utility_functions.hpp"
#include "godot_cpp/variant/variant.hpp"
#include "godot_cpp/variant/vector3.hpp"
#include "main_player.h"
#include "player_godot.h"

using namespace godot;

NetManager * net_manager;

void NetManager::_bind_methods()
{
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
    if (!Engine::get_singleton()->is_editor_hint())
    {
        // FIXME when init_networking is fixed
        ip = "127.0.0.1";
        // ip = "192.168.0.3";
        port = "1234";
        if (init_networking(/*"127.0.0.1", "1234"*/))
        {
            UtilityFunctions::print("Connected to server");
        }
        else
        {
            UtilityFunctions::print("Failed to connect");
        }
    }
}

void NetManager::_process(double delta)
{
    if (!Engine::get_singleton()->is_editor_hint())
    {
        network_tick();
    }
}

void NetManager::update_chunk(int x, int y, const chunk_table * data)
{
    String name = vformat("%d_%d", x, y);
    ChunkRenderer * chunk = Object::cast_to<ChunkRenderer>(get_node_or_null(name));
    if (!chunk)
    {
        chunk = memnew(ChunkRenderer);
        chunk->set_name(name);
        add_child(chunk);
        chunk->set_position(Vector3(x * CHUNK_SIZE, 0, y * CHUNK_SIZE));
    }
    chunk->update(data);
}

size_t my_id;

void NetManager::create_object(const ObjectData* data_c)
{
    ObjectData * data = (ObjectData *)data_c;
    switch (data->tag)
    {
        case ObjectData::Tag::Element:
        {
            UtilityFunctions::print(data->element.data.location.chunk.map_x );
            UtilityFunctions::print(data->element.data.location.chunk.map_y );
            UtilityFunctions::print(data->element.data.location.chunk.x );
            UtilityFunctions::print(data->element.data.location.chunk.y );
            ElementGodot * el = memnew(ElementGodot(data->element.data));
            add_child(el);
            el->set_position(Vector3(data->element.data.location.get_world_x() / 32, 1, data->element.data.location.get_world_y() / 32));
            register_object(el);
            break;
        }
        case ObjectData::Tag::Player:
            {
            PlayerGodot* player = nullptr; 
            if (data->player.data.uid == my_id) {
                player = memnew(MainPlayer(data->player.data));
            } else {
                player = memnew(PlayerGodot(data->player.data));
            }
            add_child(player);
            player->set_position(Vector3(data->player.data.location.get_world_x() / 32, 1, data->player.data.location.get_world_y() / 32));
            register_object(player);
            break;
        }
        default:
        {
            UtilityFunctions::print("INVALID TAG", (int)data->tag);
        }
    }
}

// FIXME remove when net.cpp is cleaned up
void update_hotbar()
{
}

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
    Class_id c_id = data->inv_element.data.c_id;

    InventoryElement * el = get_object_by_id(data->inv_element.data);

    if (el && el->c_id == c_id)
    {
        switch (c_id)
        {
            case Class_Element:
            {
                Element * element = dynamic_cast<Element *>(el);
                *element = data->element.data;
                break;
            }
            case Class_Ingredient:
            {
                Ingredient * ing = dynamic_cast<Ingredient *>(el);
                *ing = data->ingredient.data;
                break;
            }
            case Class_Product:
            {
                Product * prod = dynamic_cast<Product *>(el);
                *prod = data->product.data;
                break;
            }
            case Class_Plant:
            {
                Plant * plant = dynamic_cast<Plant *>(el);
                *plant = data->plant.data;
                // CONSOLE_LOG("%s size=%f\n", plant->get_name(), plant->size);
                break;
            }
            case Class_Animal:
            {
                Animal * animal = dynamic_cast<Animal *>(el);
                *animal = data->animal.data;
                //     CONSOLE_LOG("%s size=%f\n", animal->get_name(), animal->size);
                break;
            }
            case Class_Player:
            {
                Player * player = dynamic_cast<Player *>(el);
                CONSOLE_LOG("update_object: player=%s inv.elements=%d\n", player->get_name(), player->inventory.nr_elements);
                *player = data->player.data;
                CONSOLE_LOG("update_object: -> update: inv.elements=%d\n", player->inventory.nr_elements);
                break;
            }
            default:
                break;
        }
    }
    else
    {
        if (el)
            CONSOLE_LOG("bad data for update object %ld %d real %d", el->uid, c_id, el->c_id);
        else
            CONSOLE_LOG("non existing object for update object %ld %d", data->inv_element.data.uid, c_id);
    }
}

void update_item_location(LocationUpdateData data)
{
    InventoryElement * el = get_object_by_id(data.id);
    Node3D * node = dynamic_cast<Node3D *>(el);
    if (!node)
    {
        UtilityFunctions::print("INVALID");
        return;
    }
    if (data.old.tag == ItemLocation::Tag::Player)
    {
        node->set_visible(true);
        Player* p = (Player*)get_object_by_id(NetworkObject(Class_Player, data.old.player.id));
        p->drop(el);
        
    }
    if (data.new_.tag == ItemLocation::Tag::Player)
    {
        node->set_visible(false);
        Player* p = (Player*)get_object_by_id(NetworkObject(Class_Player, data.new_.player.id));
        p->pickup(el);
        // TODO
    }
    else
    {
        node->set_position(Vector3(data.new_.get_world_x() / 32.0, 1, data.new_.get_world_y() / 32.0));
    }
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
