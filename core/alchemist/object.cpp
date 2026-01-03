#include "object.h"

NetworkObject::NetworkObject(Class_id id) : c_id(id), uid((size_t)this) {}
NetworkObject::NetworkObject(Class_id id, size_t uid) : c_id(id), uid(uid) {}

size_t NetworkObject::get_uid() const
{
    return uid;
}

Class_id NetworkObject::get_cid() const
{
    return c_id;
}

