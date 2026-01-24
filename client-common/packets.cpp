#include "../core/packets.h"
#include <cstddef>

PacketObjectCreate::PacketObjectCreate() : Packet(PACKET_OBJECT_CREATE)
{
    obj = nullptr;
}

PacketPlayerId::PacketPlayerId() : Packet(PACKET_PLAYER_ID)
{
}

bool PacketPlayerId::update(unsigned char * net_data, size_t s)
{
    if (CHECK_SIZE(s))
    {
        pdata = (struct serial_data *)net_data;
        return true;
    }
    return false;
}

PacketObjectDestroy::PacketObjectDestroy() : Packet(PACKET_OBJECT_DESTROY)
{
}
bool PacketObjectDestroy::update(unsigned char * net_data, size_t s)
{
    if (CHECK_SIZE(s))
    {
        pdata = (struct serial_data *)net_data;
        return true;
    }
    return false;
}
unsigned long PacketObjectDestroy::get_id()
{
    return pdata->id;
}
ItemLocation PacketObjectDestroy::get_location()
{
    return pdata->location;
}

PacketElementsList::PacketElementsList() : Packet(PACKET_ELEMENTS_LIST)
{
}

bool PacketElementsList::update(unsigned char * data, size_t s)
{
    pdata = (struct serial_data *)data;
    if (s != pdata->size)
        return false;
    /*      for (int i=0; i<100; i++)
               CONSOLE_LOG("[%d] = %d %x\n", i, obj->data[i], (obj->data[i]));
    */
    //   CONSOLE_LOG("PacketElementList: list=%s elems=%d %s\n", pdata->name, pdata->nr_elements, class_name[get_list_c_id()]);
    return true;
}
int PacketElementsList::get_nr_elements()
{
    return pdata->nr_elements;
}
Class_id PacketElementsList::get_c_id()
{
    return pdata->c_id;
}
Class_id PacketElementsList::get_list_c_id()
{
    return pdata->list_c_id;
}
int PacketElementsList::get_pl_id()
{
    return pdata->pl_id;
}
unsigned char * PacketElementsList::get_data()
{
    return pdata->data;
}

bool PacketObjectCreate::update(unsigned char * data, size_t s)
{
    struct serial_data * d = (struct serial_data *)data;
    if (s != d->size)
        return false;
    obj = (ObjectData *)(&d->data);
    /*      for (int i=0; i<100; i++)
               CONSOLE_LOG("[%d] = %d %x\n", i, obj->data[i], (obj->data[i]));
    */
    // CONSOLE_LOG("PacketObjectCreate for objectData::Tag=%d size=%ld\n", (int)obj->tag, obj->size);
    switch (obj->tag)
    {
        case ObjectData::Tag::Element:
        {
            new (&obj->element.data) Element(obj->id);
            break;
        }
        case ObjectData::Tag::Field:
        {
            size_t * pdata = (size_t *)&obj->data[0];
            Field * field = static_cast<Field *>(&obj->field.data);
            //  CONSOLE_LOG("PacketObjectCreate: Field uid=%lx\n", field->get_uid());
            // CONSOLE_LOG("pdata=%lx\n", *pdata);
            new (&obj->field.data) Field((Place_id)obj->id, field->get_uid());
            // CONSOLE_LOG("PacketObjectCreate: new Field uid=%lx\n", field->get_uid());
            break;
        }
        case ObjectData::Tag::Barn:
        {
            size_t * pdata = (size_t *)&obj->data[0];
            Barn * barn = static_cast<Barn *>(&obj->barn.data);
            //  CONSOLE_LOG("PacketObjectCreate: Barn uid=%lx\n", barn->get_uid());
            new (&obj->barn.data) Barn((Place_id)obj->id, barn->get_uid());
            //  CONSOLE_LOG("PacketObjectCreate: new Barn uid=%lx\n", barn->get_uid());
            break;
        }
        case ObjectData::Tag::Plant:
        {
            new (&obj->plant.data) Plant(obj->id);
            break;
        }
        case ObjectData::Tag::Animal:
        {
            new (&obj->animal.data) Animal(obj->id);
            break;
        }
        case ObjectData::Tag::Player:
            new (&obj->player.data.inventory) ElementsList("inventory");
            new (&obj->player.data.known_elements) ElementsList("known elements");
            // new (obj->player.data.player_skills) Skills();
            new (&obj->player.data.clan) SerializablePointer<Clan>(get_clan_by_id(Clan_Human));
            // talking_to
            // relations
            break;
        default:
            break;
    }
    return true;
}

PacketObjectUpdate::PacketObjectUpdate() : Packet(PACKET_OBJECT_UPDATE)
{ // called by client
    obj = nullptr;
}

bool PacketObjectUpdate::update(unsigned char * data, size_t s)
{
    struct serial_data * d = (struct serial_data *)data;
    if (s != d->size)
        return false;
    obj = (ObjectData *)(&d->data);
    /*      for (int i=0; i<100; i++)
               CONSOLE_LOG("[%d] = %d %x\n", i, obj->data[i], (obj->data[i]));
    */
    //   CONSOLE_LOG("PacketObjectUpdate for objectData::Tag=%d\n", (int)obj->tag);
    switch (obj->tag)
    {
        case ObjectData::Tag::Element:
        {
            new (&obj->element.data) Element(obj->id);
            break;
        }
        case ObjectData::Tag::Plant:
        {
            new (&obj->plant.data) Plant(obj->id);
            break;
        }
        case ObjectData::Tag::Animal:
        {
            new (&obj->animal.data) Animal(obj->id);
            break;
        }
        case ObjectData::Tag::Player:
            //    CONSOLE_LOG("ObjectUpdate for player inv_elems=%d\n", obj->player.data.inventory.nr_elements);
            new (&obj->player.data.inventory) ElementsList("inventory");
            //    CONSOLE_LOG("ObjectUpdate for player initialized: inv_elems=%d\n", obj->player.data.inventory.nr_elements);
            new (&obj->player.data.known_elements) ElementsList("known elements");
            // new (&obj->player.data.player_skills) Skills();
            new (&obj->player.data.clan) SerializablePointer<Clan>(get_clan_by_id(Clan_Human));
            // talking_to
            // relations
            break;
        default:
            break;
    }

    return true;
}

unsigned char PacketChunkUpdate::get_x()
{
    return pdata->x;
}
unsigned char PacketChunkUpdate::get_y()
{
    return pdata->y;
}
chunk_table * PacketChunkUpdate::get_table()
{
    return ptable;
}

PacketChunkUpdate::PacketChunkUpdate() : Packet(PACKET_CHUNK_UPDATE)
{
}

bool PacketChunkUpdate::update(unsigned char * net_data, size_t s)
{
    if (CHECK_SIZE(s))
    {
        pdata = (struct serial_data *)net_data;
        ptable = &pdata->table;
        return true;
    }
    return false;
}

PacketPlayerMove::PacketPlayerMove(float x, float y) : Packet(PACKET_PLAYER_MOVE)
{
    data.t = t;
    data.x = x;
    data.y = y;
    //    CONSOLE_LOG("move: x=%f y=%f\n", x, y);
}

int PacketPlayerMove::send(ENetPeer * peer)
{
    return send_data(peer, &data, sizeof(struct serial_data));
}

LocationUpdateData PacketLocationUpdate::get_location()
{
    return pdata->location;
}

PacketLocationUpdate::PacketLocationUpdate() : Packet(PACKET_LOCATION_UPDATE)
{
}
bool PacketLocationUpdate::update(unsigned char * net_data, size_t s)
{
    if (CHECK_SIZE(s))
    {
        pdata = (struct serial_data *)net_data;
        return true;
    }
    return false;
}

PacketRequestChunk::PacketRequestChunk(int cx, int cy) : Packet(PACKET_REQUEST_CHUNK)
{
    data.t = t;
    data.cx = cx;
    data.cy = cy;
}

int PacketRequestChunk::send(ENetPeer * peer)
{
    return send_data(peer, &data, sizeof(struct serial_data));
}

PacketRequestItem::PacketRequestItem(size_t id) : Packet(PACKET_REQUEST_ITEM)
{
    data.t = t;
    data.id = id;
}

int PacketRequestItem::send(ENetPeer * peer)
{
    return send_data(peer, &data, sizeof(struct serial_data));
}

PacketPlayerActionPickup::PacketPlayerActionPickup(uintptr_t id) : Packet(PACKET_PLAYER_ACTION_PICKUP)
{
    data.t = t;
    data.id = id;
}

int PacketPlayerActionPickup::send(ENetPeer * peer)
{
    return send_data(peer, &data, sizeof(struct serial_data));
}

PacketPlayerActionDrop::PacketPlayerActionDrop(uintptr_t id) : Packet(PACKET_PLAYER_ACTION_DROP)
{
    data.t = t;
    data.id = id;
}
int PacketPlayerActionDrop::send(ENetPeer * peer)
{
    return send_data(peer, &data, sizeof(struct serial_data));
}

PacketPlayerActionUseItemOnObject::PacketPlayerActionUseItemOnObject(uintptr_t iid, uintptr_t oid) : Packet(PACKET_PLAYER_ACTION_USE_ITEM_ON_OBJECT)
{
    data.t = t;
    data.iid = iid;
    data.oid = oid;
}

int PacketPlayerActionUseItemOnObject::send(ENetPeer * peer)
{
    return send_data(peer, &data, sizeof(struct serial_data));
}

PacketPlayerActionOnObject::PacketPlayerActionOnObject(Player_action a, uintptr_t oid) : Packet(PACKET_PLAYER_ACTION_ON_OBJECT)
{
    data.t = t;
    data.a = a;
    data.oid = oid;
}

int PacketPlayerActionOnObject::send(ENetPeer * peer)
{
    return send_data(peer, &data, sizeof(struct serial_data));
}

PacketServerActionOnObject::PacketServerActionOnObject(Server_action a, uintptr_t oid) : Packet(PACKET_SERVER_ACTION_ON_OBJECT)
{
    data.t = t;
    data.a = a;
    data.oid = oid;
}
int PacketServerActionOnObject::send(ENetPeer * peer)
{
    return send_data(peer, &data, sizeof(struct serial_data));
}

PacketPlayerActionUseItemOnTile::PacketPlayerActionUseItemOnTile(uintptr_t iid, int map_x, int map_y, int x, int y) : Packet(PACKET_PLAYER_ACTION_USE_ITEM_ON_TILE)
{
    data.t = t;
    data.iid = iid;
    data.map_x = map_x;
    data.map_y = map_y;
    data.x = x;
    data.y = y;
}

int PacketPlayerActionUseItemOnTile::send(ENetPeer * peer)
{
    return send_data(peer, &data, sizeof(struct serial_data));
}

void * PacketPlayerActionCraft::serial_data::operator new(size_t size_base, size_t extra)
{
    //        CONSOLE_LOG("serial_data: allocating %ld + %ld\n", size_base, extra);
    return ::operator new(size_base + extra);
}
PacketPlayerActionCraft::serial_data::serial_data(uintptr_t prod_id, uintptr_t ing_num, const uintptr_t * iid_tab, PacketType t) : t(t), prod_id(prod_id), ing_num(ing_num)
{
    for (uintptr_t i = 0; i < ing_num; i++)
    {
        iid[i] = iid_tab[i];
    }
    size = sizeof(struct serial_data) + ing_num * sizeof(*iid_tab);
}
void PacketPlayerActionCraft::serial_data::operator delete(void * ptr)
{
    ::operator delete(ptr);
}

PacketPlayerActionCraft::PacketPlayerActionCraft(uintptr_t prod_id, uintptr_t ing_num, const uintptr_t * iid) : Packet(PACKET_PLAYER_ACTION_CRAFT)
{
    pdata = new (ing_num * sizeof(*iid)) serial_data(prod_id, ing_num, iid, t);
}

int PacketPlayerActionCraft::send(ENetPeer * peer)
{
    /*for (int i=0; i< 100; i++)
       CONSOLE_LOG("[%d] = %d %x\n", i, obj->data[i], (obj->data[i]));
*/
    return send_data(peer, pdata, pdata->size);
}

int PacketKnowledgeUpdate::get_pl_id()
{
    return pdata->pl_id;
}
Class_id PacketKnowledgeUpdate::get_cid()
{
    return pdata->cid;
}
int PacketKnowledgeUpdate::get_id()
{
    return pdata->id;
}

PacketKnowledgeUpdate::PacketKnowledgeUpdate() : Packet(PACKET_KNOWLEDGE_UPDATE)
{
}
bool PacketKnowledgeUpdate::update(unsigned char * net_data, size_t s)
{
    if (CHECK_SIZE(s))
    {
        pdata = (struct serial_data *)net_data;
        return true;
    }
    return false;
}
int PacketCheckedUpdate::get_pl_id()
{
    return pdata->pl_id;
}
uintptr_t PacketCheckedUpdate::get_id()
{
    return pdata->id;
}
PacketCheckedUpdate::PacketCheckedUpdate() : Packet(PACKET_CHECKED_UPDATE)
{
}
bool PacketCheckedUpdate::update(unsigned char * net_data, size_t s)
{
    if (CHECK_SIZE(s))
    {
        pdata = (struct serial_data *)net_data;
        return true;
    }
    return false;
}

NOT_SUPPORTED_SEND_METHOD(PacketPlayerId)
NOT_SUPPORTED_SEND_METHOD(PacketObjectDestroy)
NOT_SUPPORTED_SEND_METHOD(PacketElementsList)
NOT_SUPPORTED_SEND_METHOD(PacketObjectCreate)
NOT_SUPPORTED_SEND_METHOD(PacketObjectUpdate)
NOT_SUPPORTED_SEND_METHOD(PacketChunkUpdate)
NOT_SUPPORTED_SEND_METHOD(PacketLocationUpdate)
NOT_SUPPORTED_SEND_METHOD(PacketKnowledgeUpdate)
NOT_SUPPORTED_SEND_METHOD(PacketCheckedUpdate)

NOT_SUPPORTED_UPDATE_METHOD(PacketPlayerMove)
NOT_SUPPORTED_UPDATE_METHOD(PacketRequestChunk)
NOT_SUPPORTED_UPDATE_METHOD(PacketRequestItem)
NOT_SUPPORTED_UPDATE_METHOD(PacketPlayerActionPickup)
NOT_SUPPORTED_UPDATE_METHOD(PacketPlayerActionDrop)
NOT_SUPPORTED_UPDATE_METHOD(PacketPlayerActionUseItemOnObject)
NOT_SUPPORTED_UPDATE_METHOD(PacketPlayerActionOnObject)
NOT_SUPPORTED_UPDATE_METHOD(PacketServerActionOnObject)
NOT_SUPPORTED_UPDATE_METHOD(PacketPlayerActionUseItemOnTile)
NOT_SUPPORTED_UPDATE_METHOD(PacketPlayerActionCraft)

Packet * check_client_packet(char dir, unsigned char * data, size_t s)
{
    Packet * p = nullptr;
    switch (data[0])
    {
        case PACKET_ACTION_FAILED:
            p = new PacketActionFailed();
            break;
        case PACKET_PLAYER_ID:
            p = new PacketPlayerId();
            break;
        case PACKET_CHUNK_UPDATE:
            p = new PacketChunkUpdate();
            break;
        case PACKET_OBJECT_CREATE:
            p = new PacketObjectCreate();
            break;
        case PACKET_OBJECT_UPDATE:
            p = new PacketObjectUpdate();
            break;
        case PACKET_OBJECT_DESTROY:
            p = new PacketObjectDestroy();
            break;
        case PACKET_LOCATION_UPDATE:
            p = new PacketLocationUpdate();
            break;
        case PACKET_KNOWLEDGE_UPDATE:
            p = new PacketKnowledgeUpdate();
            break;
        case PACKET_CHECKED_UPDATE:
            p = new PacketCheckedUpdate();
            break;
        case PACKET_ELEMENTS_LIST:
            p = new PacketElementsList();
            break;
    }
    if (!p)
    {
        CONSOLE_LOG("check_packet: UNKNOWN PACKET %d with size: %ld\n", data[0], s);
        return nullptr;
    }
    if (p->update(data, s))
    {
        show_packet_type_name(dir, p->get_type());
        return p;
    }
    else
    {
        delete p;
        CONSOLE_LOG("check_packet: wrong update for %d\n", data[0]);
        return nullptr;
    }
}
