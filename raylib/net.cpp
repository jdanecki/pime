#include "../client-common/net.h"
#include "../client-common/players.h"

NetworkObject * el_from_data(const ObjectData * data)
{
   // CONSOLE_LOG("CREATING OBJECT for tag: %d\n", (int)data->tag);
    NetworkObject * el = nullptr;
    switch (data->tag)
    {
        case ObjectData::Tag::InvElement:
            break;
        case ObjectData::Tag::Element:
        {
            el = new Element(data->element.data);
            break;
        }
        case ObjectData::Tag::Place:
        {
            el = new Place(data->place.data);
            break;
        }
        case ObjectData::Tag::Scroll:
            el = new Scroll(data->scroll.data);
            break;
        case ObjectData::Tag::Ingredient:
            el = new Ingredient(data->ingredient.data);
            break;
        case ObjectData::Tag::Product:
            el = new Product(data->product.data);
            break;
        case ObjectData::Tag::Plant:
        {
            el = new Plant(data->plant.data);
            break;
        }
        case ObjectData::Tag::Animal:
            el = new Animal(data->animal.data);
            break;
        case ObjectData::Tag::Player:
            el = new PlayerUI(data->player.data);
            if (my_id == el->uid)
            {
                player = (PlayerUI *)el;
                CONSOLE_LOG("new player uid=%ld name=%s\n", player->uid, player->get_name());
            }
            break;
#ifdef DISABLE_NPC
#warning DISABLE_NPC
#endif

        case ObjectData::Tag::Npc:
#if !defined(DISABLE_NPC)
            el = new Npc(data->npc.data);
            el->c_id = Class_Npc;
            CONSOLE_LOG("creating NPC");
#endif
            break;

        case ObjectData::Tag::Clan:
            el = new Clan(data->clan.data);
            break;
        default:
            CONSOLE_LOG("UNKNOWN Tag: %d\n", (int)data->tag);
            abort();
    }
    return el;
}


