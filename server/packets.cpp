#include "../core/packets.h"

PacketObjectCreate::PacketObjectCreate(NetworkObject * el) : Packet(PACKET_OBJECT_CREATE)
{
    obj = convert_to_data(el);
}

PacketPlayerId::PacketPlayerId(unsigned long id) : Packet(PACKET_PLAYER_ID)
{
    data.t = t;
    data.id = id;
    pdata = &data;
}

int PacketPlayerId::send(ENetPeer * peer)
{
    return send_data(peer, &data, sizeof(struct serial_data));
}

PacketObjectDestroy::PacketObjectDestroy(InventoryElement * el) : Packet(PACKET_OBJECT_DESTROY)
{
    data.t = t;
    data.id = el->get_uid();
    data.location = el->location;
    el_to_remove = el;
}

int PacketObjectDestroy::send(ENetPeer * peer)
{
    // FIXME send as broadcast
    int ret = send_data(peer, &data, sizeof(struct serial_data));
    // FIXME
    //	delete el_to_remove;
    return ret;
}

void * PacketElementsList::serial_data::operator new(size_t size_base, size_t extra)
{
    //     CONSOLE_LOG("PacketElementsList: serial_data: allocating %ld + %ld\n", size_base, extra);
    return ::operator new(size_base + extra);
}
PacketElementsList::serial_data::serial_data(size_t s) : size(s)
{
}
void PacketElementsList::serial_data::operator delete(void * ptr)
{
    ::operator delete(ptr);
}
void PacketElementsList::copy_base_list_element(ListElement * el, serial_data * pdata, int i)
{
    BaseListElement * base_el = static_cast<BaseListElement *>(el);
    Base * base = static_cast<Base *>(base_el->get_el());
    base->copy_data(&pdata->data[0], i);
}
void PacketElementsList::copy_list_element(ListElement * el, serial_data * pdata, int i)
{
    NetworkObject * obj = el->get_el();
    size_t uid = obj->get_uid();
    size_t * dst = &((size_t *)(&pdata->data))[i];
    *dst = uid;
    //    CONSOLE_LOG("copy_list_element: [%d/%d]=%lx\n", i, pdata->nr_elements, uid);
}
void PacketElementsList::init(ElementsList * list)
{
    int size = list->nr_elements * list->head->get_size();
    pdata = new (size) serial_data(sizeof(serial_data) + size);
    pdata->t = t;
    pdata->nr_elements = list->nr_elements;
    strncpy(pdata->name, list->name, strlen(list->name) + 1);
    pdata->list_c_id = list->head->get_cid();

    int i = 0;
    ListElement * el = list->head;
    while (el)
    {
        switch (pdata->list_c_id)
        {
            case Class_BaseListElement:
                pdata->c_id = ((BaseListElement *)list->head)->get_el()->get_cid();
                copy_base_list_element(el, pdata, i);
                break;
            case Class_ListElement:
                copy_list_element(el, pdata, i);
                break;
        }

        el = el->next;
        i++;
    }
}
PacketElementsList::PacketElementsList(Player * pl) : Packet(PACKET_ELEMENTS_LIST)
{
    init(&pl->inventory);
    pdata->pl_id = pl->get_id();
}
PacketElementsList::PacketElementsList(ElementsList * list) : Packet(PACKET_ELEMENTS_LIST)
{
    init(list);
}

int PacketElementsList::send(ENetPeer * peer)
{
    int ret = 0;
    /*for (int i=0; i< 100; i++)
       CONSOLE_LOG("[%d] = %d %x\n", i, obj->data[i], (obj->data[i]));
*/
    ret = send_data(peer, pdata, pdata->size);
    return ret;
}

void * PacketObjectCreate::serial_data::operator new(size_t size_base, size_t extra)
{
    //        CONSOLE_LOG("PacketObjectCreate: serial_data: allocating %ld + %ld\n", size_base, extra);
    return ::operator new(size_base + extra);
}
PacketObjectCreate::serial_data::serial_data(size_t s) : size(s)
{
    //        CONSOLE_LOG("PacketObjectCreate: serial_data: set size to %ld\n", size);
}
void PacketObjectCreate::serial_data::operator delete(void * ptr)
{
    ::operator delete(ptr);
}

int PacketObjectCreate::send(ENetPeer * peer)
{
    int ret = 0;
    struct serial_data * d = new (obj->size) serial_data(sizeof(serial_data) + obj->size);
    d->t = t;
    memcpy(d->data, (void *)obj, obj->size);

    /*for (int i=0; i< 100; i++)
       CONSOLE_LOG("[%d] = %d %x\n", i, obj->data[i], (obj->data[i]));
*/
    ret = send_data(peer, d, d->size);
    delete (d);
    return ret;
}

PacketObjectUpdate::PacketObjectUpdate(InventoryElement * el) : Packet(PACKET_OBJECT_UPDATE)
{
    obj = convert_to_data(el);
}

void * PacketObjectUpdate::serial_data::operator new(size_t size_base, size_t extra)
{
    //   CONSOLE_LOG("PacketObjectUpdate: serial_data: allocating %ld + %ld\n", size_base, extra);
    return ::operator new(size_base + extra);
}
PacketObjectUpdate::serial_data::serial_data(size_t s) : size(s)
{
}
void PacketObjectUpdate::serial_data::operator delete(void * ptr)
{
    ::operator delete(ptr);
}

int PacketObjectUpdate::send(ENetPeer * peer)
{
    int ret = 0;
    struct serial_data * d = new (obj->size) serial_data(sizeof(serial_data) + obj->size);
    d->t = t;
    memcpy(d->data, (void *)obj, obj->size);

    /*for (int i=0; i< 100; i++)
       CONSOLE_LOG("[%d] = %d %x\n", i, obj->data[i], (obj->data[i]));
*/
    ret = send_data(peer, d, d->size);
    delete (d);
    return ret;
}

PacketChunkUpdate::PacketChunkUpdate(unsigned char x, unsigned char y) : Packet(PACKET_CHUNK_UPDATE)
{
    data.t = t;
    data.x = x;
    data.y = y;
    chunk_valid = false;
    if (world_table[y][x])
    {
        memcpy(&data.table, &world_table[y][x]->table, sizeof(chunk_table));
        chunk_valid = true;
    }
    else
        CONSOLE_LOG("PacketChunkUpdate: requested not loaded chunk x=%d y=%d\n", x, y);
}

int PacketChunkUpdate::send(ENetPeer * peer)
{
    int ret;
    if (chunk_valid)
    {
        ret = send_data(peer, &data, sizeof(struct serial_data));

        chunk * ch = world_table[data.y][data.x];
        // ch->objects.show(false);
        // ch->beings.show(false);
        ListElement * el = ch->objects.head;
        while (el)
        {
            Packet * p = new PacketObjectCreate(el->get_el());
            ret = p->send(peer);
            delete p;
            el = el->next;
        }
        return ret;
    }
    else
    {
        Packet * p = new PacketActionFailed();
        ret = p->send(peer);
        delete p;
        return ret;
    }
}

PacketPlayerMove::PacketPlayerMove() : Packet(PACKET_PLAYER_MOVE)
{
}

bool PacketPlayerMove::update(unsigned char * net_data, size_t s)
{
    if (CHECK_SIZE(s))
    {
        pdata = (struct serial_data *)net_data;
        //    CONSOLE_LOG("move: x=%f y=%f\n", pdata->x, pdata->y);
        return true;
    }
    return false;
}

float PacketPlayerMove::get_x()
{
    return pdata->x;
}
float PacketPlayerMove::get_y()
{
    return pdata->y;
}

PacketLocationUpdate::PacketLocationUpdate(size_t i, ItemLocation old_loc, ItemLocation new_loc) : Packet(PACKET_LOCATION_UPDATE)
{
    data.t = t;
    data.location.id = NetworkObject(Class_Unknown, i);
    data.location.old = old_loc;
    data.location.new_ = new_loc;
}

int PacketLocationUpdate::send(ENetPeer * peer)
{
    return send_data(peer, &data, sizeof(struct serial_data));
}

int PacketRequestChunk::get_cx()
{
    return pdata->cx;
}
int PacketRequestChunk::get_cy()
{
    return pdata->cy;
}

PacketRequestChunk::PacketRequestChunk() : Packet(PACKET_REQUEST_CHUNK)
{
}

bool PacketRequestChunk::update(unsigned char * net_data, size_t s)
{
    if (CHECK_SIZE(s))
    {
        pdata = (struct serial_data *)net_data;
        return true;
    }
    return false;
}

int PacketRequestItem::get_id()
{
    return pdata->id;
}

PacketRequestItem::PacketRequestItem() : Packet(PACKET_REQUEST_ITEM)
{
}
bool PacketRequestItem::update(unsigned char * net_data, size_t s)
{
    if (CHECK_SIZE(s))
    {
        pdata = (struct serial_data *)net_data;
        return true;
    }
    return false;
}

uintptr_t PacketPlayerActionPickup::get_id()
{
    return pdata->id;
}

PacketPlayerActionPickup::PacketPlayerActionPickup() : Packet(PACKET_PLAYER_ACTION_PICKUP)
{
}
bool PacketPlayerActionPickup::update(unsigned char * net_data, size_t s)
{
    if (CHECK_SIZE(s))
    {
        pdata = (struct serial_data *)net_data;
        return true;
    }
    return false;
}

uintptr_t PacketPlayerActionDrop::get_id()
{
    return pdata->id;
}

PacketPlayerActionDrop::PacketPlayerActionDrop() : Packet(PACKET_PLAYER_ACTION_DROP)
{
}
bool PacketPlayerActionDrop::update(unsigned char * net_data, size_t s)
{
    if (CHECK_SIZE(s))
    {
        pdata = (struct serial_data *)net_data;
        return true;
    }
    return false;
}

uintptr_t PacketPlayerActionUseItemOnObject::get_iid()
{
    return pdata->iid;
}
uintptr_t PacketPlayerActionUseItemOnObject::get_oid()
{
    return pdata->oid;
}

PacketPlayerActionUseItemOnObject::PacketPlayerActionUseItemOnObject() : Packet(PACKET_PLAYER_ACTION_USE_ITEM_ON_OBJECT)
{
}
bool PacketPlayerActionUseItemOnObject::update(unsigned char * net_data, size_t s)
{
    if (CHECK_SIZE(s))
    {
        pdata = (struct serial_data *)net_data;
        return true;
    }
    return false;
}

Player_action PacketPlayerActionOnObject::get_a()
{
    return pdata->a;
}
uintptr_t PacketPlayerActionOnObject::get_oid()
{
    return pdata->oid;
}

PacketPlayerActionOnObject::PacketPlayerActionOnObject() : Packet(PACKET_PLAYER_ACTION_ON_OBJECT)
{
}
bool PacketPlayerActionOnObject::update(unsigned char * net_data, size_t s)
{
    if (CHECK_SIZE(s))
    {
        pdata = (struct serial_data *)net_data;
        return true;
    }
    return false;
}

Server_action PacketServerActionOnObject::get_a()
{
    return pdata->a;
}
uintptr_t PacketServerActionOnObject::get_oid()
{
    return pdata->oid;
}

PacketServerActionOnObject::PacketServerActionOnObject() : Packet(PACKET_SERVER_ACTION_ON_OBJECT)
{
}
bool PacketServerActionOnObject::update(unsigned char * net_data, size_t s)
{
    if (CHECK_SIZE(s))
    {
        pdata = (struct serial_data *)net_data;
        return true;
    }
    return false;
}

int PacketPlayerActionUseItemOnTile::get_map_x()
{
    return pdata->map_x;
}
int PacketPlayerActionUseItemOnTile::get_map_y()
{
    return pdata->map_y;
}
int PacketPlayerActionUseItemOnTile::get_x()
{
    return pdata->x;
}
int PacketPlayerActionUseItemOnTile::get_y()
{
    return pdata->y;
}
uintptr_t PacketPlayerActionUseItemOnTile::get_iid()
{
    return pdata->iid;
}

PacketPlayerActionUseItemOnTile::PacketPlayerActionUseItemOnTile() : Packet(PACKET_PLAYER_ACTION_USE_ITEM_ON_TILE)
{
}
bool PacketPlayerActionUseItemOnTile::update(unsigned char * net_data, size_t s)
{
    if (CHECK_SIZE(s))
    {
        pdata = (struct serial_data *)net_data;
        return true;
    }
    return false;
}

uintptr_t PacketPlayerActionCraft::get_prod_id()
{
    return pdata->prod_id;
}
uintptr_t PacketPlayerActionCraft::get_ing_num()
{
    return pdata->ing_num;
}
uintptr_t * PacketPlayerActionCraft::get_iid_table()
{
    return pdata->iid;
}

PacketPlayerActionCraft::PacketPlayerActionCraft() : Packet(PACKET_PLAYER_ACTION_CRAFT)
{
}

bool PacketPlayerActionCraft::update(unsigned char * data, size_t s)
{
    pdata = (struct serial_data *)data;
    if (s != pdata->size)
        return false;
    /*      for (int i=0; i<100; i++)
               CONSOLE_LOG("[%d] = %d %x\n", i, pdata->data[i], (pdata->data[i]));
    */
    return true;
}

PacketKnowledgeUpdate::PacketKnowledgeUpdate(int pl_id, Class_id cid, int id) : Packet(PACKET_KNOWLEDGE_UPDATE)
{
    data.t = t;
    data.pl_id = pl_id;
    data.cid = cid;
    data.id = id;
}

int PacketKnowledgeUpdate::send(ENetPeer * peer)
{
    return send_data(peer, &data, sizeof(struct serial_data));
}

PacketCheckedUpdate::PacketCheckedUpdate(int pl_id, uintptr_t id) : Packet(PACKET_CHECKED_UPDATE)
{
    data.t = t;
    data.pl_id = pl_id;
    data.id = id;
}

int PacketCheckedUpdate::send(ENetPeer * peer)
{
    return send_data(peer, &data, sizeof(struct serial_data));
}

NOT_SUPPORTED_UPDATE_METHOD(PacketPlayerId)
NOT_SUPPORTED_UPDATE_METHOD(PacketObjectDestroy)
NOT_SUPPORTED_UPDATE_METHOD(PacketElementsList)
NOT_SUPPORTED_UPDATE_METHOD(PacketObjectCreate)
NOT_SUPPORTED_UPDATE_METHOD(PacketObjectUpdate)
NOT_SUPPORTED_UPDATE_METHOD(PacketChunkUpdate)
NOT_SUPPORTED_UPDATE_METHOD(PacketLocationUpdate)
NOT_SUPPORTED_UPDATE_METHOD(PacketKnowledgeUpdate)
NOT_SUPPORTED_UPDATE_METHOD(PacketCheckedUpdate)

NOT_SUPPORTED_SEND_METHOD(PacketPlayerMove)
NOT_SUPPORTED_SEND_METHOD(PacketRequestChunk)
NOT_SUPPORTED_SEND_METHOD(PacketRequestItem)
NOT_SUPPORTED_SEND_METHOD(PacketPlayerActionPickup)
NOT_SUPPORTED_SEND_METHOD(PacketPlayerActionDrop)
NOT_SUPPORTED_SEND_METHOD(PacketPlayerActionUseItemOnObject)
NOT_SUPPORTED_SEND_METHOD(PacketPlayerActionOnObject)
NOT_SUPPORTED_SEND_METHOD(PacketServerActionOnObject)
NOT_SUPPORTED_SEND_METHOD(PacketPlayerActionUseItemOnTile)
NOT_SUPPORTED_SEND_METHOD(PacketPlayerActionCraft)

Packet * check_server_packet(char dir, unsigned char * data, size_t s)
{
    Packet * p = nullptr;
    switch (data[0])
    {
        case PACKET_JOIN_REQUEST:
            p = new PacketJoinRequest();
            break;
        case PACKET_PLAYER_MOVE:
            p = new PacketPlayerMove();
            break;
        case PACKET_REQUEST_CHUNK:
            p = new PacketRequestChunk();
            break;
        case PACKET_PLAYER_ACTION_PICKUP:
            p = new PacketPlayerActionPickup();
            break;
        case PACKET_PLAYER_ACTION_DROP:
            p = new PacketPlayerActionDrop();
            break;
        case PACKET_PLAYER_ACTION_USE_ITEM_ON_OBJECT:
            p = new PacketPlayerActionUseItemOnObject();
            break;
        case PACKET_PLAYER_ACTION_ON_OBJECT:
            p = new PacketPlayerActionOnObject();
            break;
        case PACKET_SERVER_ACTION_ON_OBJECT:
            p = new PacketServerActionOnObject();
            break;
        case PACKET_PLAYER_ACTION_USE_ITEM_ON_TILE:
            p = new PacketPlayerActionUseItemOnTile();
            break;
        case PACKET_PLAYER_ACTION_CRAFT:
            p = new PacketPlayerActionCraft();
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

ObjectData * convert_to_data(NetworkObject * el)
{
    ObjectData * obj = nullptr;
    //  CONSOLE_LOG("convert_to_data: c_id=%d uid=%lx\n", el->c_id, el->get_uid());
    switch (el->c_id)
    {
        case Class_Element:
        {
            Element * element = static_cast<Element *>(el);
            obj = new ObjectData(ObjectData::Tag::Element);
            obj->element.data = *element;
            obj->id = element->get_id();
            break;
        }
        case Class_Field:
        {
            Field * field = static_cast<Field *>(el);
            obj = new ObjectData(ObjectData::Tag::Field);
            obj->field.data = *field;
            obj->id = field->get_id();
            break;
        }
        case Class_Barn:
        {
            Barn * barn = static_cast<Barn *>(el);
            obj = new ObjectData(ObjectData::Tag::Barn);
            obj->barn.data = *barn;
            //   CONSOLE_LOG("Barn: uid=%lx\n", barn->get_uid());
            obj->id = barn->get_id();
            break;
        }
        case Class_Player:
        {
            Player * player = static_cast<Player *>(el);
            obj = new ObjectData(ObjectData::Tag::Player);
            obj->player.data = *player;
            if (player->inventory.nr_elements)
                add_packet_to_send1(new PacketElementsList(player));
            // obj->player.data.clan = nullptr;
            // obj->player.data.player_skills = nullptr;
            // obj->player.data.inventory = nullptr;
            // obj->player.data.known_elements = nullptr;
            // obj->player.data.talking_to = nullptr;
            // obj->player.data.relations = nullptr;
            break;
        }
        case Class_Npc:
        {
            Npc * npc = static_cast<Npc *>(el);
            obj = new ObjectData(ObjectData::Tag::Npc);
            obj->npc.data = *npc;
            break;
        }
        case Class_Ingredient:
        {
            Ingredient * ing = static_cast<Ingredient *>(el);
            obj = new ObjectData(ObjectData::Tag::Ingredient);
            obj->ingredient.data = *ing;
            break;
        }
        case Class_Product:
        {
            Product * prod = static_cast<Product *>(el);
            obj = new ObjectData(ObjectData::Tag::Product);
            obj->product.data = *prod;
            break;
        }
        case Class_Plant:
        {
            Plant * plant = static_cast<Plant *>(el);
            obj = new ObjectData(ObjectData::Tag::Plant);
            obj->plant.data = *plant;
            obj->id = plant->get_id();
            break;
        }
        case Class_Animal:
        {
            Animal * animal = static_cast<Animal *>(el);
            obj = new ObjectData(ObjectData::Tag::Animal);
            obj->animal.data = *animal;
            obj->id = animal->get_id();
            break;
        }
        case Class_Scroll:
        {
            Scroll * scroll = static_cast<Scroll *>(el);
            obj = new ObjectData(ObjectData::Tag::Scroll);
            obj->scroll.data = *scroll;
            break;
        }

        default:
            //    CONSOLE_LOG("Unknown class ID=%d in convert_to_data\n", el->c_id);
            assert(0);
            break;
    }
    return obj;
}
