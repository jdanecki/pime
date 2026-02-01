#include "element_server.h"
#include "../elements_server.h"
#include "../../core/player.h"

ElementServer * create_element(BaseElement * base)
{
    //  CONSOLE_LOG("create_element %s\n", base->get_name());
    return new ElementServer(base);
}

ElementServer::ElementServer(BaseElement * b) : Element(b)
{
}
bool ElementServer::action(Product_action action, Player * pl)
{
    CONSOLE_LOG("ELEMENT_SERVER: %s %s\n", product_action_name[action], get_name());

    bool res = false;
    switch (action)
    {
        case ACT_CUT:
            res = action_cut();
            break;
        case ACT_HIT:
            res = action_hit();
            break;
    }
    if (dimensions.volume.value < 1)
    {
        destroy(this);
    }
    else
    {
        notify_update(this);
    }
    return res;
}

bool ElementServer::action_cut()
{
    BaseElement * b = (BaseElement *)get_base();
    if (b->form == Form_solid)
    {
        //    if (b->solid->hardness < 50)
        {
            dimensions.volume.value = dimensions.length.decrease(1) * dimensions.width.decrease(1) * dimensions.height.decrease(1);
            mass.value = b->density.value * dimensions.volume.value / 1000;
        }

        return true;
    }
    return false;
}

bool ElementServer::action_hit()
{
    BaseElement * b = (BaseElement *)get_base();
    if (b->form == Form_solid)
    {
        //    if (b->solid->hardness < 50)
        {
            dimensions.volume.value = dimensions.length.decrease(3) * dimensions.width.decrease(3) * dimensions.height.decrease(3);
            mass.value = b->density.value * dimensions.volume.value / 1000;
        }

        return true;
    }
    CONSOLE_LOG("ELEMENT_SERVER: action_hit failed: wrong form %s\n", Form_name[b->form]);
    return false;
}

bool ElementServer::player_action(Player_action action, Player * pl)
{
    bool res = false;

    CONSOLE_LOG("ELEMENT_SERVER: %s %s\n", player_action_name[action], get_name());
    switch (action)
    {
        case PLAYER_DRINK:
            res = action_drink();
            if (res)
            {
                pl->thirst += 10;
            }
            break;
        case PLAYER_EAT:
            res = action_eat();
            if (res)
            {
                pl->hunger += 10;
            }
            break;
    }

    if (dimensions.volume.value < 1)
    {
        destroy(this);
    }
    else
    {
        notify_update(this);
    }
    return res;
}

bool ElementServer::action_drink()
{
    BaseElement * b = (BaseElement *)get_base();
    if (b->form == Form_liquid)
    {
        //    if (b->solid->hardness < 50)
        {
            dimensions.volume.value = dimensions.length.decrease(2) * dimensions.width.decrease(2) * dimensions.height.decrease(2);
            mass.value = b->density.value * dimensions.volume.value / 1000;
            CONSOLE_LOG("drunk %s\n", get_name());
        }
        return true;
    }
    else
        CONSOLE_LOG("can't drink %s\n", get_name());
    return false;
}

bool ElementServer::action_eat()
{
    BaseElement * b = (BaseElement *)get_base();
    if (b->form == Form_solid)
    {
        //    if (b->solid->hardness < 50)
        {
            dimensions.volume.value = dimensions.length.decrease(4) * dimensions.width.decrease(4) * dimensions.height.decrease(4);
            mass.value = b->density.value * dimensions.volume.value / 1000;
            CONSOLE_LOG("ate %s\n", get_name());
        }
        return true;
    }
    else
        CONSOLE_LOG("can't eat %s\n", get_name());
    return false;
}

void ElementServer::show(bool details)
{
    Element::show(details);
}

bool ElementServer::can_pickup()
{
    // return mass.value < 10000;
    return true;
}
