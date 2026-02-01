#include "scroll_server.h"
#include "../../core/player.h"
#include "../elements_server.h"

ScrollServer::ScrollServer(Base * b) : Scroll(b)
{
}

bool ScrollServer::player_action(Player_action action, Player * pl)
{
    CONSOLE_LOG("ScrollServer: %s %s\n", player_action_name[action], get_name());

    switch (action)
    {
        case PLAYER_READ:
            if (pl->set_known(get_base_cid(), get_id()))
            {
                notify_knowledge(pl->get_id(), get_base_cid(), get_id());
            }
            else
            {
                CONSOLE_LOG("ScrollServer: already known this item\n");
            }
            destroy(this);
            break;
    }

    return true;
}

bool ScrollServer::can_pickup()
{
    return true;
}

ScrollServer * create_scroll(Base * base)
{
    // CONSOLE_LOG("create_scroll: base=%s id=%d\n", base->get_name(), base->id);
    ScrollServer * s = new ScrollServer(base);
    return s;
}
