#include "being_server.h"

void BeingServer::show(bool details)
{
    age->show();
    max_age->show();
}

bool BeingServer::grow()
{
    if (!alive)
    {
        return false;
    }
    age->value++;

    if (age->value >= max_age->value)
    {
        alive = false;
    }
    return alive;
}

BeingServer::BeingServer()
{
    max_age = new Property("max age", 0);
    age = new Property("age", 1);
    alive = true;
}
Property ** BeingServer::get_properties(int * count)
{
    Property ** props = new Property *[2];
    props[0] = age;
    props[1] = max_age;

    *count = 2;
    return props;
}
BeingServer::~BeingServer()
{
    delete age;
    delete max_age;
}
bool BeingServer::tick()
{
    return grow();
}