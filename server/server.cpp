#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include "server.h"
#include "../core/alchemist/el_list.h"
#include "player_server.h"
#include "world_server.h"
#include "craft.h"
#include "places/places.h"
#include "networking.h"
#include <math.h>
#include <ncurses.h>
#include <sys/ioctl.h>
#include "generator/generator.h"

//#define DEBUG_TIMEOUT 1

#define RED "$COLOR_PAIR_1_"
#define GREEN "$COLOR_PAIR_2_"
#define ORANGE "$COLOR_PAIR_3_"
#define BLUE "$COLOR_PAIR_4_"
#define RESET "$COLOR_PAIR_0_"
#define INPUT_SIZE 64
#define MAX_OUTPUT_SIZE 1024
int history_size = 1024;

WINDOW * in_w;
WINDOW * out_w;
WINDOW * status;

char in_buf[INPUT_SIZE + 1];
char ** out_buf;

int history_up;
int buf_pos;
bool scrolling=true;

void shift_output()
{
    for (int i = 1; i < history_size; i++)
    {
        strncpy(out_buf[i - 1], out_buf[i], MAX_OUTPUT_SIZE);
        out_buf[i][0]=0;
    }
}

int print_color(WINDOW * w, const char * string)
{
    int current_color = 0;
    const char * c = string;
    if (!(*c)) return 2;
    int ret = OK;
    while (*c)
    {        
        if (*c == '$' && strncmp(c, "$COLOR_PAIR_", 12) == 0)
        {
            c += 12;
            current_color = atoi(c);
            while (*c != '_')
                c++;
            c++;
            if (!*c)
                break;
        }
        wattron(w, COLOR_PAIR(current_color));
        if (waddch(w, *c) == ERR) ret = 1;
        wattroff(w, COLOR_PAIR(current_color));
        if (ret == 1) break;
        c++;
    }
    return ret;
}

int add_to_output(const char * fmt, ...)
{
    va_list args;
    va_start(args, fmt);

    vsnprintf(out_buf[buf_pos], MAX_OUTPUT_SIZE, fmt, args);

    if (buf_pos < history_size-1) buf_pos++;
    else shift_output();
    va_end(args);

    return 0;
}

void print_help()
{
    add_to_output("e - exit\n");
    add_to_output("h - help\n");
    add_to_output("c - clear\n");
    add_to_output("p - players\n");
    add_to_output("1 - show terrains types\n");
    add_to_output("2 - show plants types\n");
    add_to_output("3 - show animals types\n");
    add_to_output("4 - show chunk\n");
    add_to_output("5 - show base elements\n");
    add_to_output("6 - show base plants\n");
    add_to_output("7 - show base animals\n");
}

void clear_history()
{
    for (int i = 0; i < history_size; i++)
    {
        memset(out_buf[i], 0, MAX_OUTPUT_SIZE);
    }
    history_up=0;
    buf_pos=0;
    add_to_output("%scleared%s\n", RED, RESET);
}

void handle_resize()
{
    int w, h;
    getmaxyx(stdscr, h, w);
    wresize(out_w, h - 4, w);
    wresize(in_w, 3, w);
    mvwin(in_w, h - 4, 0);
}

char handle_command();
char handle_pressed(int pressed)
{
    if (pressed == 0)
        return 0;

    if (pressed < 127 && (isalpha((unsigned char)pressed) || isdigit((unsigned char)pressed)))
    {
        char append[2] = {(char)pressed, 0};
        strcat(in_buf, append);
    }
    switch (pressed)
    {
        case KEY_BACKSPACE:
            if (strlen(in_buf))
            {
                in_buf[strlen(in_buf) - 1] = '\0';
            }
            break;
        case '\n':
        {
            add_to_output("> %s\n", in_buf);
            char retval = handle_command();
            in_buf[0] = '\0';
            return retval;
        }
        case KEY_RESIZE:
            handle_resize(); // FALLTHROUGH
        case KEY_END:
            scrolling=true;
            break;
        case KEY_HOME:
            scrolling=false;
            history_up = 0;
            break;
        case KEY_DOWN:
            scrolling=false;
            history_up += 5;
            break;
        case KEY_UP:
            scrolling=false;
            history_up -= 5;
            if (history_up < 0)
                history_up = 0;
            break;
    }
    return 0;
}

char ncurses_init()
{
    initscr();
    start_color();
    cbreak();
    noecho();
    keypad(stdscr, TRUE);
    curs_set(1);

    int w, h;
    getmaxyx(stdscr, h, w);
    out_w = newwin(h - 4, w, 0, 0);
    in_w = newwin(3, w, h - 4, 0);
    status = newwin(1, w, h - 1, 0);
    wbkgd(status, COLOR_PAIR(5));
    wbkgd(in_w, COLOR_PAIR(6));
 //   keypad(out_w, TRUE);
    keypad(in_w, TRUE);
  //  nodelay(out_w, TRUE);
    nodelay(in_w, TRUE);

    out_buf = (char **)calloc(history_size, sizeof(char *));
    for (int i = 0; i < history_size; i++)
    {
        out_buf[i] = (char *)calloc(MAX_OUTPUT_SIZE, sizeof(char *));
    }

    memset(in_buf, 0, sizeof(char) * INPUT_SIZE);

    init_pair(0, COLOR_WHITE, COLOR_BLACK);
    init_pair(1, COLOR_RED, COLOR_BLACK);
    init_pair(2, COLOR_GREEN, COLOR_BLACK);
    init_pair(3, COLOR_YELLOW, COLOR_BLACK);
    init_pair(4, COLOR_BLUE, COLOR_BLACK);
    init_pair(5,COLOR_BLACK,COLOR_BLUE);
    init_pair(6,COLOR_BLACK,COLOR_GREEN);
    scrollok(out_w, FALSE);

    return 0;
}


void ncurses_tick()
{
    werase(in_w);
    werase(out_w);
    box(in_w, 0, 0);
    wmove(in_w, 1, 2);
    char buf[128];
    snprintf(buf, 128, "> %s", in_buf);
    print_color(in_w, buf);    

    for (int i = 0; i + history_up < history_size; i++)
    {
        int ret = print_color(out_w, out_buf[i+history_up]);
        switch(ret)
        {
            case 1:
                if (scrolling) {
                    history_up++;
                }
            case 2:
                goto last_line;
        }
    }
last_line:
    wrefresh(out_w);
    wmove(in_w, 4 + strlen(in_buf), 1);
    wrefresh(in_w);    
}

Region ** regions;

ElementsList base_elements("base elements");
ElementsList base_plants("base plants");
ElementsList base_animals("base animals");

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
        add_to_output("player uid=%ld host=%s port=%u\n", player->uid, hostname, peer->address.port);
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
        add_to_output("[%d/%d] sending to player: %d\n", i++, players->nr_elements, pl->player->get_id());
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

void print_status(int l, const char * format, ...)
{
    va_list args;
    char buf[128];
    va_start(args, format);
    vsnprintf(buf, 127, format, args);
    va_end(args);

    wprintw(status, "\r %s ", buf);
    wrefresh(status);
}

bool handle_packet(ENetPacket * packet, ENetPeer * peer)
{
    unsigned char * data = packet->data;
   // add_to_output("Received length=%lu: %d\n", packet->dataLength, *data);

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
                    add_to_output("use product: %s on tile [%d,%d]:[%d,%d]\n", prod->get_name(), map_x, map_y, x, y);
                    if (!pl->player->use_product_on_tile(prod,  map_x, map_y, x, y))
                    {
                        p = new PacketActionFailed();
                        p->send(peer);
                        delete p;
                    }
                }
                else
                {
                    add_to_output("use item: %s on tile [%d,%d]:[%d,%d]\n", item->get_name(), map_x, map_y, x, y);
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
                add_to_output("crafted id=%ld\n", prod_id);
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
//        add_to_output("sending updates elems=%d\n", packets_to_send->nr_elements);
        while (el)
        {
            PacketToSend * p = (PacketToSend *)el;
            p->to_all();
            el = el->next;
        }
//        add_to_output("sent updates\n");
    }

    if (packets_to_send1->nr_elements)
    {
        ListElement * el = packets_to_send1->head;
       // add_to_output("sending updates1 elems=%d\n", packets_to_send1->nr_elements);
        while (el)
        {
            PacketToSend * p = (PacketToSend *)el;
            p->to_all();
            el = el->next;
        }
      //  add_to_output("sent updates1\n");
    }

    if (objects_to_create.nr_elements)
    {
        ListElement * el = objects_to_create.head;
        while (el)
        {
//            add_to_output("sending objects to create: %s id=%lx\n", static_cast<InventoryElement*> (el->el.get())->get_name(), el->el.get()->uid);
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
    add_to_output("load_chunk(%d, %d)\n", cx, cy);
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
        add_to_output("empty chunk, trying again %d\n", r->rocks_count);
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
            add_to_output("%sinvalid command: %s%s\n", RED, RESET, in_buf);
            add_to_output("use \"help\" to show help\n");
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
    add_to_output("Copyright (C) 2025 Piotr Danecki <i3riced@mailfence.com>\n");
    add_to_output("Copyright (C) 2025 Jacek Danecki\n");
    add_to_output("This program comes with ABSOLUTELY NO WARRANTY.\n");
    add_to_output("This is free software, and you are welcome to redistribute it under certain conditions.\n");
    add_to_output("See LICENSE file\n");
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
    add_to_output("Server Pime started on port %u\n", address.port);

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
        if (ch != ERR && handle_pressed(ch))
            break;
        switch (event.type)
        {
            case ENET_EVENT_TYPE_CONNECT:
            {
                enet_address_get_host_ip(&event.peer->address, hostname, 512);
                add_to_output("Client connected %s:%d\n", hostname, event.peer->address.port);
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

                add_to_output("player: %s from %s:%d disconnected\n", pl->player->get_name(), hostname, event.peer->address.port);
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
