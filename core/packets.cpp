#include "packets.h"

ObjectData * convert_to_data(NetworkObject * el)
{
    ObjectData * obj = nullptr;
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
        case Class_Place:
        {
            Place * place = static_cast<Place *>(el);
            // obj = new ObjectData(ObjectData::Tag::Place);
            size_t new_size = sizeof(struct ObjectData) + sizeof(size_t);
            obj = new (new_size) ObjectData(ObjectData::Tag::Place, new_size);
            obj->place.data = *place;
            obj->id = place->get_id();
            size_t * pdata = (size_t *)&obj->data[0];
            *pdata = place->get_uid();
            //     CONSOLE_LOG("pdata=%lx size=%d\n", *pdata, obj->size);
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
        case Class_Npc:
        {
            Npc * npc = static_cast<Npc *>(el);
            obj = new ObjectData(ObjectData::Tag::Npc);
            obj->npc.data = *npc;
            break;
        }
        default:
            //    CONSOLE_LOG("Unknown class ID=%d in convert_to_data\n", el->c_id);
            assert(0);
            break;
    }
    return obj;
}

Packet * check_packet(char dir, unsigned char * data, size_t s)
{
    Packet * p = nullptr;
    switch (data[0])
    {
        case PACKET_JOIN_REQUEST:
            p = new PacketJoinRequest();
            break;
        case PACKET_ACTION_FAILED:
            p = new PacketActionFailed();
            break;
        case PACKET_PLAYER_ID:
            p = new PacketPlayerId();
            break;
        case PACKET_PLAYER_MOVE:
            p = new PacketPlayerMove();
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
    if (p->update(data, s))
    {
        show_packet_type_name(dir, p->get_type());
        return p;
    }
    else
    {
        delete p;
        CONSOLE_LOG("UNKNOWN PACKET: %d with size: %ld\n", data[0], s);
        return nullptr;
    }
}
