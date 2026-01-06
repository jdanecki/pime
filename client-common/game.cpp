#include <stdio.h>
#include <stdlib.h>

#include "game.h"
#include "../core/alchemist/ncurses-output.h"
#include "../core/world.h"

#include "../core/time_core.h"

extern NetClient * client;

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
        clear_window();

        if (handle_events())
            return;

        if (auto_explore)
        {
            do_auto_explore();
        }
        draw();
    }
}

void init_game(const char * name, int argc, char * argv[])
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
    if (setup(ip, port))
    {
        print_status(0, "Welcome in %s!", name);
        loop();
    }
}
