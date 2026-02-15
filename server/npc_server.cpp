#include "npc_server.h"
#include "elements/being_server.h"
#include "networking.h"
#include "../core/alchemist/random_functions.h"

ElementsList * npcs;
int npcs_id = 100;

NPCServer::NPCServer(size_t uid) : Npc(uid, ItemLocation::center())
{
    // CONSOLE_LOG("NPCServer: uid=%ld\n", uid);
    location.chunk.x = random_range(0, CHUNK_SIZE);
    location.chunk.y = random_range(0, CHUNK_SIZE);

    notify_create(this);
    dst_loc.chunk.map_x = location.chunk.map_x;
    dst_loc.chunk.map_y = location.chunk.map_y;
    dst_loc.chunk.x = random_range(0, CHUNK_SIZE);
    dst_loc.chunk.y = random_range(0, CHUNK_SIZE);
    target = nullptr;
    state = IDLE;
}

void NPCServer::find_plant()
{
    float dist;
    target = find_closest_item(this, &dist, Class_Plant);
    if (target)
    {
        dst_loc = target->location;
        CONSOLE_LOG("NPC(%s) finding plant @(%d,%d - %f,%f) cid=%d\n", get_name(), dst_loc.chunk.map_x, dst_loc.chunk.map_y, dst_loc.chunk.x, dst_loc.chunk.y, target->get_cid());
        state = FINDING_FOOD;
    }
    else
    {
        CONSOLE_LOG("NPC(%s) discovering\n", get_name());
        state = DISCOVERING;
        dst_loc = location;
    }
}
void NPCServer::check_state()
{
    //   CONSOLE_LOG("NPCServer::check_state: state=%d\n", state);
    ItemLocation old_location = location;
    switch (state)
    {
        case IDLE:
        {
            find_plant();
            break;
        }
        case FINDING_FOOD:
        {
            if (target->location.tag == ItemLocation::Tag::Chunk)
            {
                move_to(this);
                if (distance(location.get_world_x(), location.get_world_y(), target->location.get_world_x(), target->location.get_world_y()) < 0.5)
                {
                    pickup(target);
                    state = IDLE;
                    dst_loc = location;
                }
            }
            else
            {
                state = IDLE;
                dst_loc = location;
            }
            break;
        }
        case DISCOVERING:
        {
            discover(this);
            find_plant();
            break;
        }
        case CONVERSATION:
        break;
    }
    if (old_location != location)
    {
        update_location(NetworkObject(get_cid(), get_uid()), old_location, location);
    }
}

bool NPCServer::tick()
{
    if (check_move())
    {
        check_state();
    }
    //  Player::tick();
    return true;
}

void NPCServer::show(bool details)
{
    Npc::show(details);
    CONSOLE_LOG("NPCServer: state=%d\ndst_loc: ", state);
    dst_loc.show();
    BeingServer::show(details);
}

void NPCServer::pickup(InventoryElement * item)
{
    ItemLocation old_location = item->location;
    remove_from_chunks(item);
    Player::pickup(item);
    update_location(NetworkObject(item->get_cid(), item->get_uid()), old_location, item->location);
}

bool NPCServer::player_action(Player_action action, Player *pl)
{
    bool res = false;
    CONSOLE_LOG("NPC_SERVER: %s %s\n", player_action_name[action], get_name());
    NpcState cur=state;
    switch(action)
    {
        case PLAYER_NPC_START_CONVERSATION:
            CONSOLE_LOG("%s start talking to %s\n", pl->get_name(), get_name());
            state = CONVERSATION;
            start_conversation(pl);
            pl->start_conversation(this);
        break;
        case PLAYER_NPC_STOP_CONVERSATION:
            CONSOLE_LOG("%s stopped talking to %s\n", pl->get_name(), get_name());
            state = IDLE;
            stop_conversation();
            pl->stop_conversation();
        break;
    }
    if (cur != state) {
        notify_update(pl);
        notify_update(this);
    }
    return res;
}

void show_npcs()
{
    ListElement * pl_el = npcs->head;
    while (pl_el)
    {
        NPCServer * pl = static_cast<NPCServer *>(pl_el->get_el());
        pl->show();
        pl_el = pl_el->next;
    }
}
void create_npcs()
{
    npcs = new ElementsList("NPCs");
    NPCServer * npc1 = new NPCServer(npcs_id++);
    npcs->add(npc1);
    add_object_to_world(npc1);
    /*
        NPCServer * npc2 = new NPCServer(npcs_id++);
        npcs->add(npc2);
        add_object_to_world(npc2);*/
}
