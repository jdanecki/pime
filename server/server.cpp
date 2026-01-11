#include <stdio.h>
#include <stdarg.h>
#include "../core/packets.h"
#include "../core/alchemist/el_list.h"
#include "player_server.h"
#include "world_server.h"
#include "craft.h"
#include "places/places.h"
#include "networking.h"
#include <math.h>
//#include <sys/ioctl.h>
#include "console.h"
#include "generator/generator.h"

#define DEBUG_TIMEOUT 1

void print_help()
{
    CONSOLE_LOG("e - exit\n");
    CONSOLE_LOG("h - help\n");
    CONSOLE_LOG("c - clear\n");
    CONSOLE_LOG("p - players\n");
    CONSOLE_LOG("1 - show terrains types\n");
    CONSOLE_LOG("2 - show plants types\n");
    CONSOLE_LOG("3 - show animals types\n");
    CONSOLE_LOG("4 - show chunk\n");
    CONSOLE_LOG("5 - show base elements\n");
    CONSOLE_LOG("6 - show base plants\n");
    CONSOLE_LOG("7 - show base animals\n");
    CONSOLE_LOG("# - show keys\n");
}

char handle_command();
char handle_pressed(int pressed)
{
    if (pressed == 0)
        return 0;

    switch (pressed)
    {
        case KEY_BACKSPACE:
        case 8:
            if (strlen(in_buf))
            {
                in_buf[strlen(in_buf) - 1] = '\0';
            }
            break;
        case '\n':
        {
            CONSOLE_LOG("> %s\n", in_buf);
            char retval = handle_command();
            in_buf[0] = '\0';
            return retval;
        }
        case KEY_RESIZE:
            handle_resize(); // FALLTHROUGH
        case KEY_END:
            scrolling=true;
            history_up=buf_pos-win_h+1;
            if (history_up < 0) history_up=0;
            break;
        case KEY_HOME:
            scrolling=false;
            history_up = 0;
            break;
        case KEY_DOWN:
            scrolling=false;
            if (history_up < (buf_pos-5)) history_up += 5;
            break;
        case KEY_UP:
            scrolling=false;
            history_up -= 5;
            if (history_up < 0)
                history_up = 0;
            break;
        case '#':
             show_key^=1;
            break;
        default:
            char append[2] = {(char)pressed, 0};
            strcat(in_buf, append);
            break;
    }
    return 0;
}

Region ** regions;

BaseElement * get_base_element(size_t id)
{
    BaseListElement * el = (BaseListElement *)base_elements.find(&id);
    if (!el)
        return nullptr;
    return (BaseElement *)((el)->base);
}

BasePlant * get_base_plant(size_t id)
{
    BaseListElement * el = (BaseListElement *)base_plants.find(&id);
    if (!el)
        return nullptr;
    return (BasePlant *)((el)->base);
}

BaseAnimal * get_base_animal(size_t id)
{
    BaseListElement * el = (BaseListElement *)base_animals.find(&id);
    if (!el)
        return nullptr;
    return (BaseAnimal *)((el)->base);
}

ElementsList *players, *packets_to_send, *packets_to_send1;
InvList objects_to_create;
int players_id;

ENetHost * server;

struct Peer_id
{
    enum class Tag
    {
        Peer,
        Id,
    };
    Tag tag;
    union
    {
        ENetPeer * peer;
        unsigned long id;
    };
};

class PlayerClient : public ListElement
{
  public:
    PlayerServer * player;
    ENetPeer * peer;
    PlayerClient(PlayerServer * player, ENetPeer * peer) : player(player), peer(peer)
    {
        show();
    }
    bool check(void * what)
    {
        Peer_id * p = (Peer_id *)what;
        switch (p->tag)
        {
            case Peer_id::Tag::Peer:
                return p->peer == peer;
            case Peer_id::Tag::Id:
                return p->id == player->uid;
        }
        return false;
    }
    void show(bool details = true)
    {
        char hostname[64];
        enet_address_get_host_ip(&peer->address, hostname, 64);
        CONSOLE_LOG("player uid=%ld host=%s port=%u\n", player->uid, hostname, peer->address.port);
        player->location.show();
    }
};

//#define SEND_LOG=1
void send_to_all(Packet * p)
{
    ListElement * pl_el = players->head;
#ifdef SEND_LOG
    int i = 0;
#endif
    while (pl_el)
    {
        PlayerClient * pl = (PlayerClient *)pl_el;
#ifdef SEND_LOG
        CONSOLE_LOG("[%d/%d] sending to player: %d\n", i++, players->nr_elements, pl->player->get_id());
#endif
        p->send(pl->peer);
        pl_el = pl_el->next;

    }
}

class PacketToSend : public ListElement
{
    Packet * p;

  public:
    PacketToSend(Packet * p) : p(p)
    {
    }
    void send(ENetPeer * peer)
    {
        p->send(peer);
    }
    void to_all()
    {
        send_to_all(p);
    }
};

bool handle_packet(ENetPacket * packet, ENetPeer * peer)
{
    unsigned char * data = packet->data;
   // CONSOLE_LOG("Received length=%lu: %d\n", packet->dataLength, *data);

    Packet * p = check_packet('R', data, packet->dataLength);
    if (!p)
        return false;

    Peer_id peer_id;
    peer_id.tag = Peer_id::Tag::Peer;
    peer_id.peer = peer;
    PlayerClient * pl = nullptr;
    if (players->nr_elements)
        pl = (PlayerClient *)players->find(&peer_id);

    switch (p->get_type())
    {
        case PACKET_JOIN_REQUEST:
        {
            delete p;
            p = new PacketPlayerId(players_id++);
            p->send(peer);
            PlayerClient * new_pl = new PlayerClient(new PlayerServer(((PacketPlayerId *)p)->get_id()), peer);
            players->add(new_pl);
            delete p;
            p = new PacketElementsList(&base_elements);
            p->send(peer);
            delete p;
            p = new PacketElementsList(&base_plants);
            p->send(peer);
            delete p;
            p = new PacketElementsList(&base_animals);
            p->send(peer);
            delete p;

            p = new PacketChunkUpdate(128, 128);
            p->send(peer);
            delete p;
            add_object_to_world(new_pl->player, new_pl->player->location);
            break;
        }
        case PACKET_PLAYER_MOVE:
        {
            PacketPlayerMove * move = dynamic_cast<PacketPlayerMove *>(p);
            pl->player->move(move->get_x(), move->get_y());
            delete p;
            break;
        }
        case PACKET_REQUEST_CHUNK:
        {
            PacketRequestChunk * req = dynamic_cast<PacketRequestChunk *>(p);
            int cx = req->get_cx();
            int cy = req->get_cy();
            delete p;
            p = new PacketChunkUpdate(cx, cy);
            p->send(peer);
            delete p;
            break;
        }
        case PACKET_PLAYER_ACTION_PICKUP:
        {
            PacketPlayerActionPickup * req = dynamic_cast<PacketPlayerActionPickup *>(p);
            uintptr_t id = req->get_id();
            InventoryElement * el = find_in_world(&pl->player->location, id);
            delete p;
            if (el)
            {
                if (!pl->player->pickup(el))
                {
                    p = new PacketActionFailed();
                    p->send(peer);
                    delete p;
                }
            }
            break;
        }
        case PACKET_PLAYER_ACTION_DROP:
        {
            PacketPlayerActionDrop * req = dynamic_cast<PacketPlayerActionDrop *>(p);
            uintptr_t id = req->get_id();
            InventoryElement * el = pl->player->get_item_by_uid(id);
            delete p;
            if (el)
            {
                ItemLocation location = el->location;
                add_object_to_world(el, pl->player->location);
                pl->player->drop(el);
                update_location(*el, location, el->location);
            }
            break;
        }
        case PACKET_PLAYER_ACTION_USE_ITEM_ON_OBJECT:
        {
            PacketPlayerActionUseItemOnObject * req = dynamic_cast<PacketPlayerActionUseItemOnObject *>(p);
            uintptr_t iid = req->get_iid();
            uintptr_t oid = req->get_oid();
            InventoryElement * el = pl->player->get_item_by_uid(iid);
            delete p;
            if (el)
            {
                InventoryElement * obj = find_in_world(&pl->player->location, oid);
                CONSOLE_LOG("use item on object: oid=%lx obj=%p\n", oid, obj);
                if (obj && !pl->player->use_item_on_object(el, obj))
                {
                    p = new PacketActionFailed();
                    p->send(peer);
                    delete p;
                }
            }
            break;
        }
        case PACKET_PLAYER_ACTION_ON_OBJECT:
        {
            PacketPlayerActionOnObject * req = dynamic_cast<PacketPlayerActionOnObject *>(p);
            Player_action a = req->get_a();
            uintptr_t oid = req->get_oid();
            delete p;
            InventoryElement * obj = find_in_world(&pl->player->location, oid);
            if (obj)
            {
                if (!pl->player->action_on_object(a, obj))
                {
                    p = new PacketActionFailed();
                    p->send(peer);
                    delete p;
                }
            }
            break;
        }
        case PACKET_SERVER_ACTION_ON_OBJECT:
        {
            PacketServerActionOnObject * req = dynamic_cast<PacketServerActionOnObject *>(p);
            Server_action a = req->get_a();
            uintptr_t oid = req->get_oid();
            delete p;
            InventoryElement * obj = find_in_world(&pl->player->location, oid);
            if (obj)
            {
                if (!pl->player->server_action_on_object(a, obj))
                {
                    p = new PacketActionFailed();
                    p->send(peer);
                    delete p;
                }
            }
            break;
        }
        case PACKET_PLAYER_ACTION_USE_ITEM_ON_TILE:
        {
            PacketPlayerActionUseItemOnTile * req = dynamic_cast<PacketPlayerActionUseItemOnTile *>(p);
            int map_x = req->get_map_x();
            int map_y = req->get_map_y();
            int x = req->get_x();
            int y = req->get_y();
            uintptr_t iid = req->get_iid();
            delete p;
            InventoryElement * item = pl->player->get_item_by_uid(iid);
            if (item)
            {
                if (Product * prod = dynamic_cast<Product *>(item))
                {
                    CONSOLE_LOG("use product: %s on tile [%d,%d]:[%d,%d]\n", prod->get_name(), map_x, map_y, x, y);
                    if (!pl->player->use_product_on_tile(prod,  map_x, map_y, x, y))
                    {
                        p = new PacketActionFailed();
                        p->send(peer);
                        delete p;
                    }
                }
                else
                {
                    CONSOLE_LOG("use item: %s on tile [%d,%d]:[%d,%d]\n", item->get_name(), map_x, map_y, x, y);
                    /*if (!pl->player->plant_with_seed(item, map_x, map_y, x, y))
                    {
                        p = new PacketActionFailed();
                        p->send(peer);
                        delete p;
                    }*/
                }
            }
            break;
        }
        case PACKET_PLAYER_ACTION_CRAFT:
        {
            PacketPlayerActionCraft * req = dynamic_cast<PacketPlayerActionCraft *>(p);
            uintptr_t prod_id = req->get_prod_id();
            uintptr_t ing_num = req->get_ing_num();
            uintptr_t * iid_table = req->get_iid_table();
            delete p;
            if (craft_entry(prod_id, ing_num, iid_table, pl->player))
            {
                CONSOLE_LOG("crafted id=%ld\n", prod_id);
                InventoryElement * el1 = pl->player->get_item_by_uid(iid_table[0]);
                if (el1)
                {
                    p = new PacketObjectDestroy(el1->uid, el1->location);
                    p->send(peer);
                    delete p;

                    pl->player->drop(el1);
                }
                if (prod_id >= ING_COUNT)
                {
                    InventoryElement * el2 = pl->player->get_item_by_uid(iid_table[1]);
                    if (el2)
                    {
                        p = new PacketObjectDestroy(el2->uid, el2->location);
                        p->send(peer);
                        pl->player->drop(el2);
                    }
                }
            }
            else
            {
                p = new PacketActionFailed();
                p->send(peer);
                delete p;
            }
            break;
        }
    }
    return true;
}

void send_updates()
{
    if (!players->nr_elements)
        return;

    if (packets_to_send->nr_elements)
    {
        ListElement * el = packets_to_send->head;
//        CONSOLE_LOG("sending updates elems=%d\n", packets_to_send->nr_elements);
        while (el)
        {
            PacketToSend * p = (PacketToSend *)el;
            p->to_all();
            el = el->next;
        }
//        CONSOLE_LOG("sent updates\n");
    }

    if (packets_to_send1->nr_elements)
    {
        ListElement * el = packets_to_send1->head;
       // CONSOLE_LOG("sending updates1 elems=%d\n", packets_to_send1->nr_elements);
        while (el)
        {
            PacketToSend * p = (PacketToSend *)el;
            p->to_all();
            el = el->next;
        }
      //  CONSOLE_LOG("sent updates1\n");
    }

    if (objects_to_create.nr_elements)
    {
        ListElement * el = objects_to_create.head;
        while (el)
        {
//            CONSOLE_LOG("sending objects to create: %s id=%lx\n", static_cast<InventoryElement*> (el->el.get())->get_name(), el->el.get()->uid);
            Packet * p = new PacketObjectCreate(el->el.get());
            send_to_all(p);
            el = el->next;
        }
    }

    packets_to_send->remove_all();
    packets_to_send1->remove_all();
    objects_to_create.remove_all();
}

int random_bool(double probability)
{
    return ((double)rand() / RAND_MAX) < probability;
}

typedef void (*callback_fn)(chunk * ch, size_t id);

void do_times(float prob, callback_fn f, chunk * ch, int id)
{
    int count = (int)(prob * 2.0f);
    for (int i = 0; i < count; ++i)
    {
        if (random_bool(0.5))
        {
            f(ch, id);
        }
    }
}

void add_place(chunk * ch, Place_id id, int x, int y)
{
    ch->add_object(create_place(id), x, y);
}

void add_element(chunk * ch, size_t id)
{
    BaseListElement * base_el = (BaseListElement *)base_elements.find(&id);
    ch->add_object(create_element((BaseElement *)(base_el->base)));
}

void add_plant(chunk * ch, size_t id)
{
    BaseListElement * base_el = (BaseListElement *)base_plants.find(&id);
    ch->add_object(create_plant((BasePlant *)(base_el->base)));
}

void add_animal(chunk * ch, size_t id)
{
    BaseListElement * base_el = (BaseListElement *)base_animals.find(&id);
    ch->add_object(create_animal((BaseAnimal *)(base_el->base)));
}

void load_chunk(int cx, int cy)
{
    CONSOLE_LOG("load_chunk(%d, %d)\n", cx, cy);
    chunk * ch = new chunk(cx, cy);
    Region * r = find_region(cx, cy);

try_again:
    for (int i = 0; i < r->rocks_count; i++)
    {
        do_times(r->rocks_types[i]->value, add_element, ch, r->rocks_types[i]->terrain->id);
    }
    for (int i = 0; i < r->plants_count; i++)
    {
        do_times(r->plants_types[i]->value, add_plant, ch, r->plants_types[i]->plant->id);
    }

    if (!ch->objects.nr_elements) {
        CONSOLE_LOG("empty chunk, trying again %d\n", r->rocks_count);
        goto try_again;
    }

    for (int y = 0; y < CHUNK_SIZE; y++)
        for (int x = 0; x < CHUNK_SIZE; x++)
        {
            InventoryElement * closest_el=nullptr;
            int min_dist = INT_MAX;
            ListElement * cur=ch->objects.head;
            while(cur)
            {
                InventoryElement * el=static_cast<InventoryElement *>(cur->el.get());
                int dx = x - el->location.get_x();
                int dy = y - el->location.get_y();
                int dist=dx * dx + dy * dy;
                if (dist < min_dist)
                {
                    min_dist = dist;
                    closest_el=el;
                }
                cur=cur->next;
            }
            if (closest_el) {
                ch->table[y][x].tile = closest_el->get_id();
            }
        }



    for (int i = 0; i < r->animals_count; i++)
    {
        do_times(r->animals_types[i]->value, add_animal, ch, r->animals_types[i]->animal->id);
    }

    // ch->add_object(create_scroll(new Base(rand() % 10, Class_Scroll,"scroll")));
#if 0
    int id;

    for (id=0; id < all_base_elements_count; id++)
    {
        BaseListElement * el = (BaseListElement *)base_elements.find(&id);
        ch->add_object(create_element((BaseElement *)(el->base)), id % 17, 14 + (id / 17));
    }

    for (id=0; id < BASE_PLANTS; id++)
    {
        BaseListElement * pl = (BaseListElement *)base_plants.find(&id);
        ch->add_object(create_plant((BasePlant *)(pl->base)), id % CHUNK_SIZE, 3*(id / CHUNK_SIZE));
    }

    for (id=0; id < BASE_ANIMALS; id++)
    {
        BaseListElement * an = (BaseListElement *)base_animals.find(&id);
        ch->add_object(create_animal((BaseAnimal *)(an->base)), id % CHUNK_SIZE, id / CHUNK_SIZE);
    }
#endif
    world_table[cy][cx] = ch;
}

void add_packet_to_send(Packet * p)
{
    packets_to_send->add(new PacketToSend(p));
}

void add_packet_to_send1(Packet * p)
{    
    packets_to_send1->add(new PacketToSend(p));
}

void notify_update(const InventoryElement * el)
{
    add_packet_to_send(new PacketObjectUpdate((InventoryElement *)el));
}

void notify_create(const InventoryElement * el)
{
    //add_packet_to_send(new PacketObjectCreate((InventoryElement *)el));
    objects_to_create.add((InventoryElement*)el);
}

void update_location(NetworkObject id, ItemLocation old_loc, ItemLocation new_loc)
{
    /*CONSOLE_LOG("update location uid=%lx old_tag=%d new_tag=%d\n", id, (int)old_loc.tag, (int)new_loc.tag);
     old_loc.show();
     new_loc.show();
 */
    add_packet_to_send(new PacketLocationUpdate(id.uid, old_loc, new_loc));
}
void notify_destroy(NetworkObject id, ItemLocation location)
{
    add_packet_to_send(new PacketObjectDestroy(id.uid, location));
}
void notify_knowledge(size_t pl_id, Class_id cid, int id)
{
    add_packet_to_send(new PacketKnowledgeUpdate(pl_id, cid, id));
}
void notify_checked(size_t pl_id, size_t el)
{
    add_packet_to_send(new PacketCheckedUpdate(pl_id, el));
}

void generate()
{
    players = new InvList("Players");
    create_regions();

    for (int i = 0; i < all_base_elements_count; i++)
    {
        Form f;
        int id;

        if (i < terrains_count)
        {
            f= (Form)terrains[i]->form;
            id = terrains[i]->id;
        }
        else
        {
            f= (Form) (1 + rand() % 3);
            id = i;
        }
        ListElement * entry = new BaseListElement(new BaseElement(f, id));
        base_elements.add(entry);
    }
    for (int i = 0; i < all_plants_count; i++)
    {
        ListElement * entry = new BaseListElement(new BasePlant(all_plants[i]->id));
        base_plants.add(entry);
    }
    for (int i = 0; i < all_animals_count; i++)
    {
        ListElement * entry = new BaseListElement(new BaseAnimal(all_animals[i]->id));
        base_animals.add(entry);
    }
}

void show_players()
{
    ListElement * pl_el = players->head;
    while (pl_el)
    {
        PlayerClient * pl = (PlayerClient *)pl_el;
        pl->show();
        pl_el = pl_el->next;
    }
}

void show_terrains()
{
    for (int i=0; i < terrains_count; i++)
    {
        terrains[i]->show();
    }
}

void show_plants()
{
    for (int i=0; i < all_plants_count; i++)
    {
        all_plants[i]->show();
    }
}

void show_animals()
{
    for (int i=0; i < all_animals_count; i++)
    {
        all_animals[i]->show();
    }
}

void show_chunk()
{
    ListElement * pl_el = players->head;
    if (!pl_el) return;
    PlayerClient * pl = (PlayerClient *)pl_el;
    int x = pl->player->location.chunk.map_x;
    int y = pl->player->location.chunk.map_y;
    CONSOLE_LOG("Player@[%d,%d]\n", x, y);
    chunk * ch = world_table[y][x];
    ch->show();
    Region * reg = find_region(x, y);
    reg->show();
}

char handle_command()
{    
    if (!in_buf[0]) return 0;
    switch (in_buf[0])
    {
        case 'e':
            return 1;
        case 'h':
            print_help();
            break;
        case 'c':
            clear_history();
            break;
        case 'p':
            show_players();
            break;
        default:
            CONSOLE_LOG("invalid command: %s\n", in_buf);
            CONSOLE_LOG("use \"help\" to show help\n");
            break;
        case '1':
            show_terrains();
            break;
        case '2':
            show_plants();
            break;
        case '3':
            show_animals();
            break;
        case '4':
            show_chunk();
            break;
        case '5':
            base_elements.show();
            break;
        case '6':
             base_plants.show();
            break;
        case '7':
             base_animals.show();
            break;
    }

    return 0;
}


int main()
{
    ncurses_init();
    print_status(0, "pime_enet\n");
    CONSOLE_LOG("Copyright (C) 2025 Piotr Danecki <i3riced@mailfence.com>\n");
    CONSOLE_LOG("Copyright (C) 2025 Jacek Danecki\n");
    CONSOLE_LOG("This program comes with ABSOLUTELY NO WARRANTY.\n");
    CONSOLE_LOG("This is free software, and you are welcome to redistribute it under certain conditions.\n");
    CONSOLE_LOG("See LICENSE file\n");
 //   trace_network = 1;

    srand(0);

    if (enet_initialize() != 0)
    {
        fprintf(stderr, "Can't initialize ENet\n");
        return 1;
    }
    atexit(enet_deinitialize);

    ENetAddress address;
    address.host = ENET_HOST_ANY;
    address.port = 1234;

    server = enet_host_create(&address, 32, 2, 0, 0);
    // 32 clients, 2 channels, bandwidth in, bandwidth out
    // 0-255 channels
    if (!server)
    {
        fprintf(stderr, "Can't create host\n");
        return 1;
    }
    CONSOLE_LOG("Server Pime started on port %u\n", address.port);

    generate();

    for (int cy = 127; cy < 130; cy++)
        for (int cx = 127; cx < 130; cx++)
            load_chunk(cy, cx);

    packets_to_send = new ElementsList("packets to send");
    packets_to_send1 = new ElementsList("packets1 to send");

    ENetEvent event;
    char hostname[512] = {
        0,
    };

    while (enet_host_service(server, &event, 20) >= 0)
    {
        ncurses_tick();
        int ch = wgetch(in_w);
        if (show_key) {
            if (ch!=ERR) CONSOLE_LOG("key=%d ", ch);
        }
        if (ch != ERR && handle_pressed(ch))
            break;
        switch (event.type)
        {
            case ENET_EVENT_TYPE_CONNECT:
            {
                enet_address_get_host_ip(&event.peer->address, hostname, 512);
                CONSOLE_LOG("Client connected %s:%d\n", hostname, event.peer->address.port);
                unsigned short * port = new unsigned short;
                *port = event.peer->address.port;
                event.peer->data = (void *)port;
#if DEBUG_TIMEOUT
                enet_peer_timeout(event.peer, 5 * 60 * 1000, 5 * 60 * 1000, 5 * 60 * 1000); // 5 minutes
#endif
                break;
            }
            case ENET_EVENT_TYPE_RECEIVE:
            {
                handle_packet(event.packet, event.peer);
                enet_packet_destroy(event.packet);
                send_updates();
                break;
            }
            case ENET_EVENT_TYPE_DISCONNECT:
            {
                Peer_id peer_id;
                peer_id.tag = Peer_id::Tag::Peer;
                peer_id.peer = event.peer;
                PlayerClient * pl = (PlayerClient *)players->find(&peer_id);
                enet_address_get_host_ip(&event.peer->address, hostname, 512);

                CONSOLE_LOG("player: %s from %s:%d disconnected\n", pl->player->get_name(), hostname, event.peer->address.port);
                destroy(pl->player);
                players->remove(pl);
                send_updates();
                event.peer->data = NULL;
                break;
            }
            default:
                // CONSOLE_LOG("time=%ld\n", get_time_ms());
                update();
                send_updates();
                break;
        }
    }
    delwin(in_w);
    delwin(out_w);
    endwin();

    enet_host_destroy(server);
    return 0;
}
