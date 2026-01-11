#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "backend.inl"

#include "game.h"
#include "../core/world.h"
#include "../core/time_core.h"

extern NetClient * client;
char game_name[30];

bool use_network = true;

extern bool setup(const char * ip, const char * port);
extern void clear_window();
extern void draw();
extern bool handle_events();
extern void print_status(int i, const char * format, ...);

unsigned int total_recv = 0;
unsigned int max_recv;
unsigned long max_time;
bool show_received;
int auto_explore;

// remove it from window.h
#define PANEL_WINDOW 540
#define STATUS_LINES (2 * 32)

extern int load_font();
extern void create_menus();
extern void close_graphics();
extern int init_window(const char *title, int wx, int wy);
extern void load_textures();

void put_element()
{
    InventoryElement * el = player->hotbar[active_hotbar];
    if (el)
    {
        send_packet_drop(client, el->uid);
        // player->hotbar[active_hotbar] = NULL;
        /*el->set_posittion(player.x, player.y);
        set_item_at_ppos(el, &player);
        player.inventory->remove(el);
        player.hotbar[active_hotbar]=NULL;
       CONSOLE_LOG("item %s placed\n", el->get_name());*/
    }
}

void handle_network()
{
    unsigned long start = get_time_usec();
    unsigned int recv = 0;
    if (use_network)
    {
        recv = network_tick(client);
        total_recv += recv;
        unsigned long stop = get_time_usec();
        if (recv > max_recv)
            max_recv = recv;
        if (stop - start > max_time)
            max_time = stop - start;
        if (recv && show_received)
            printf("recv: %u/%u max=%u time=%lu us/%lu ms\n", recv, total_recv, max_recv, stop - start, max_time / 1000);
    }
}

int dst_map_x;
int dst_map_y;

void do_auto_explore()
{
    if ((dst_map_x == player->location.chunk.map_x) && (dst_map_y == player->location.chunk.map_y))
    {
        int dx = 5 - (rand() % 11);
        int dy = 5 - (rand() % 11);

        if (player->location.chunk.map_y + dy >= 0 && player->location.chunk.map_y + dy < WORLD_SIZE && player->location.chunk.map_x + dx >= 0 && player->location.chunk.map_x + dx < WORLD_SIZE)
        {
            if (!world_table[player->location.chunk.map_y + dy][player->location.chunk.map_x + dx])
            {
                dst_map_x = player->location.chunk.map_x + dx;
                dst_map_y = player->location.chunk.map_y + dy;
            }
        }
    }
    if (dst_map_x > player->location.chunk.map_x)
        send_packet_move(client, 1, 0);
    if (dst_map_x < player->location.chunk.map_x)
        send_packet_move(client, -1, 0);
    if (dst_map_y > player->location.chunk.map_y)
        send_packet_move(client, 0, 1);
    if (dst_map_y < player->location.chunk.map_y)
        send_packet_move(client, 0, -1);
}

void loop()
{
    dst_map_x = player->location.chunk.map_x;
    dst_map_y = player->location.chunk.map_y;

    for (;;)
    {
        handle_network();

        if (handle_events())
            return;

        if (auto_explore)
        {
            do_auto_explore();
        }
        draw();
        if (!auto_explore) Backend_Wait();
    }
}

int init_graphics()
{
    int texture_size=32;
    if (init_window(game_name, texture_size*CHUNK_SIZE + PANEL_WINDOW, texture_size*CHUNK_SIZE + STATUS_LINES))
    {
        return 1;
    }

    create_menus();
    //map = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_ABGR8888, SDL_TEXTUREACCESS_STREAMING, WORLD_SIZE, WORLD_SIZE);

    return 0;
}


bool setup(const char * ip, const char * port)
{
    setbuf(stdout, nullptr); // fix for qtcreator console

    if (init_graphics() != 0)
        return false;

    client = init(ip, port);
    if (client) return true;
    else return false;
}


void start_game(const char * name, int argc, char * argv[])
{
    const char * ip;
    if (argc < 2)
    {
        CONSOLE_LOG("usage: ./%s <ip> [port]\n", name);
        CONSOLE_LOG("using localhost 127.0.0.1\n");
        ip = "127.0.0.1";
    }
    else
    {
        ip = argv[1];
    }
    const char * port;
    if (argc < 3)
    {
        port = "1234";
    }
    else
    {
        port = argv[2];
    }
    strcpy(game_name, name);
    if (setup(ip, port))
    {
        print_status(0, "Welcome in %s!", name);
        loop();
    }
    close_graphics();
}
