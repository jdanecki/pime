#include <string.h>

#include "../core/packets.h"

const void * data;
size_t size;

void add_packet_to_send1(Packet * p)
{}

int enet_peer_send(ENetPeer *, enet_uint8, ENetPacket *)
{
    return 0;
}

ENetPacket * enet_packet_create(const void * d, size_t s, enet_uint32)
{
    printf("send: %p size=%ld\n", d, s);
    data = new char[s];
    memcpy((void*)data, d, s);
    size = s;
    return nullptr;
}

void enet_host_flush (ENetHost *)
{}

void register_object(NetworkObject * o);

int main(int argc, char *argv[])
{
    //server side
    Npc * npc = new Npc(2, ItemLocation::center());
    Player *pl = new Player(1, SerializableCString("player"), ItemLocation::center(), 100, 101, 102);
    pl->start_conversation(npc);

    Element * el1 = new Element(new BaseElement(Form_solid, 3));
    pl->pickup(el1);
    Element * el2 = new Element(new BaseElement(Form_solid, 4));
    pl->pickup(el2);

    register_object(el1);
    register_object(el2);

    printf("player->show\n");
    pl->show();
    pl->inventory.show(false);
    printf("--- end of inventory ---\n");
    PacketObjectUpdate *pu = new PacketObjectUpdate(pl);
    ENetPeer peer;
    pu->send(&peer);
    delete pl;
    delete npc;

    printf("\nclient side\n");
    unsigned char * recv_data = (unsigned char*) data;
    if (recv_data[0] == PACKET_OBJECT_UPDATE)
    {
        PacketObjectUpdate *p = new PacketObjectUpdate();
        if (p->update(recv_data, size))
        {
            printf("%d -> %s\n",  p->get_type(), p->get_type() < PACKET_COUNT ? packet_type_names[p->get_type()] : "ERROR" );
        }
        else
        {
            printf("check_packet: wrong update for %d\n", recv_data[0]);
            return 0;
        }

        ObjectData * obj_data = p->obj;
        NetworkObject nobj = obj_data->inv_element.data;
        Player *player = new Player(obj_data->player.data);
        player->show();
        player->inventory.show(false);
    }
    else
    {
        printf("wrong data\n");
    }

    return 0;
}
