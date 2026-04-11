#include "../core/packets.h"


ElementsList all_objects("objects");

class ObjectElement : public ListElement
{
  public:
    ObjectElement(InventoryElement * el) : ListElement(el)
    {
    }
    bool check(void * what)
    {
        uintptr_t * uid = (uintptr_t *)what;
        return *uid == get_el()->uid;
    }
};

void register_object(NetworkObject * o)
{
    ObjectElement * obj = new ObjectElement((InventoryElement *)o);
    // printf("register_object: uid=%lx\n", o->uid);
    all_objects.add(obj);
}

NetworkObject * get_object_by_id(NetworkObject obj)
{
    ListElement * el = all_objects.find(&obj.uid);
    return el ? el->get_el() : nullptr;
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
        {
            int inv_elems = obj->player.data.inventory.nr_elements;
            CONSOLE_LOG("ObjectUpdate for player inv_elems=%d size=%ld\n", inv_elems, obj->size);
            new (&obj->player.data.inventory) ElementsList("inventory");
            if (inv_elems)
            {
            	ElementsList * inv = &obj->player.data.inventory;
                PacketElementsList::serial_data * pdata = (PacketElementsList::serial_data*)(&obj->data[0]);
                CONSOLE_LOG("nr_elements=%d list_c_id=%d\n", pdata->nr_elements, pdata->list_c_id);
                if (pdata->list_c_id == Class_ListElement)
                {
                    for (int i=0; i < inv_elems; i++)
                    {
                        size_t uid=((size_t*)(pdata->data))[i];
                        CONSOLE_LOG("inv[%d]=%lx\n", i, uid);
                        InventoryElement * el = static_cast<InventoryElement*>(get_object_by_id(NetworkObject(Class_Element, uid)));
						if (el) {
							  inv->add(el);
						}
                    }
                }
            }
            new (&obj->player.data.known_elements) ElementsList("known elements");
            new (&obj->player.data.talking_to) SerializablePointer<Player>();
            // relations
            break;
        }
        case ObjectData::Tag::Npc:
        {

        	break;
        }
        default:
            break;
    }

    return true;
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

