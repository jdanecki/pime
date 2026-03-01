#include "../core/packets.h"

ObjectData * convert_to_data(NetworkObject * el)
{
    ObjectData * obj = nullptr;
    //   CONSOLE_LOG("convert_to_data: c_id=%d uid=%lx\n", el->c_id, el->get_uid());
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
            if (player->inventory.nr_elements)
            {
                PacketElementsList *p_inv = new PacketElementsList(player);
                CONSOLE_LOG("p_inv: size=%ld\n", p_inv->get_size());
                obj = new (p_inv->get_size()) ObjectData(ObjectData::Tag::Player, sizeof(ObjectData) + p_inv->get_size());
                memcpy(&obj->data[0], p_inv->get_pdata(), p_inv->get_size());
            }
            else
                obj = new ObjectData(ObjectData::Tag::Player);
            obj->player.data = *player;
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

PacketObjectUpdate::PacketObjectUpdate(InventoryElement * el) : Packet(PACKET_OBJECT_UPDATE)
{
    obj = convert_to_data(el);
}

void * PacketObjectUpdate::serial_data::operator new(size_t size_base, size_t extra)
{
    CONSOLE_LOG("PacketObjectUpdate: serial_data: allocating %ld + %ld\n", size_base, extra);
    return ::operator new(size_base + extra);
}
PacketObjectUpdate::serial_data::serial_data(PacketType t, size_t s) : t(t), size(s)
{
    CONSOLE_LOG("PacketObjectUpdate::serial_data: size=%ld\n", size);
}
void PacketObjectUpdate::serial_data::operator delete(void * ptr)
{
    ::operator delete(ptr);
}

int PacketObjectUpdate::send(ENetPeer * peer)
{
    int ret = 0;
    struct serial_data * d = new (obj->size) serial_data(t, sizeof(serial_data) + obj->size);
    memcpy(d->data, (void *)obj, obj->size);

    /*for (int i=0; i< 100; i++)
       CONSOLE_LOG("[%d] = %d %x\n", i, obj->data[i], (obj->data[i]));
*/
    ret = send_data(peer, d, d->size);
    delete (d);
    return ret;
}

void * PacketElementsList::serial_data::operator new(size_t size_base, size_t extra)
{
    CONSOLE_LOG("PacketElementsList: serial_data: allocating serial_data=%ld extra=%ld\n", size_base, extra);
    return ::operator new(size_base + extra);
}
PacketElementsList::serial_data::serial_data(PacketType t, size_t s, int nr_elements) :
		t(t), size(s), nr_elements(nr_elements)
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
    CONSOLE_LOG("copy_list_element: [%d/%d]=%lx\n", i, pdata->nr_elements, uid);
}
void PacketElementsList::init(ElementsList * list)
{
    int size = list->nr_elements * list->head->get_size();
    CONSOLE_LOG("PacketElementList:init size=%ld->%d x %d, serial_data size=%d all=%ld\n",
                size, list->nr_elements, list->head->get_size(), sizeof(serial_data),
                size+sizeof(serial_data));
    pdata = new (size) serial_data(t, sizeof(serial_data) + size, list->nr_elements);
//      strncpy(pdata->name, list->name, strlen(list->name) + 1);
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

size_t PacketElementsList::get_size()
{
    return pdata->size;
}
