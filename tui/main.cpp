#include <sys/ioctl.h>
#include <termios.h>
#include <cstdlib>
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <termios.h>
#include <unistd.h>
#include "../core/world.h"
#include "../core/tiles.h"
#include "../core/networking.h"
#include "../core/player.h"
#include "../core/time.h"
#include "playerTUI.h"


#define PlayerTYPE PlayerTUI
#include "../client-common/net.inl"

extern int trace_network;
bool use_network = true;
bool show_received;
unsigned int max_recv;
unsigned long max_time;

bool finish;

NetClient * client;

struct termios old_stdin, stdin_tty;
void set_terminal()
{
    setbuf(stdout, nullptr);
    tcgetattr(0, &old_stdin);
    stdin_tty = old_stdin;
    stdin_tty.c_lflag &= ~(ECHO | ICANON | ECHOE); // | ISIG);
    stdin_tty.c_cc[VMIN] = 1;
    stdin_tty.c_cc[VTIME] = 0;
    tcsetattr(0, TCSANOW, &stdin_tty);

    tcflush(0, TCIFLUSH);
    ioctl(0, TCXONC, 1);
}

NetClient * setup(const char * ip, const char * port)
{
    set_terminal();

    client = init(ip, port);
    return client;
}

void print_status(int l, const char * format, ...)
{
    va_list args;
    va_start(args, format);
    if (!l)
        vprintf(format, args);
    else
        vprintf(format, args);
    va_end(args);
    puts("");
}

int kbhit()
{
    int lf;

    if (ioctl(0, FIONREAD, &lf) == 0)
        return lf;
    else
        return -1;
}

char wait_key(char prompt)
{
    printf("\r [%c] > ", prompt);
    while (!kbhit())
    {
        usleep(1000);
    }
    char c;
    if (read(0, &c, 1) == 1)
    {
        printf("%c\n", c);
        return c;
    }
    else
        return 0;
}

char check_key()
{
    if (!kbhit())
        return 0;
    char c;
    if (read(0, &c, 1) == 1)
    {
        printf("%c\n", c);
        return c;
    }
    else
        return 0;
}

void help()
{
    printf("? - show\n");
    printf("1 .. 0 - select n-element in hotbar\n");
    printf("h - help\n");
    printf("i/I - pickup/drop item at player location\n");
    printf("n - use/don't use network\n");
    printf("q - quit\n");
    printf("r - show counter received packets per tick\n");
    printf("t/T - trace network in client/server\n");
    printf("wasd - move player\n");
}

void help_question_mark()
{
    printf("b - show hotbar\n");
    printf("c/C - show chunk in client/server\n");
    printf("e/E - show item at player location in client/server\n");
    printf("h - help\n");
    printf("i - show inventory\n");
    printf("o - show objects\n");
    printf("B - show base elements\n");
    printf("p/P - show player/players\n");
    printf("r - return to main menu\n");
}

char submenu;
bool do_key_question_mark(char k)
{
    switch (k)
    {
        case 'b':
            for (int i = 0; i < 10; i++)
            {
                if (player->hotbar[i])
                {
                    printf("%d: ", i);
                    player->hotbar[i]->show();
                }
            };
            break;
        case 'B':
            base_elements.show(true);
            break;
        case 'c':
            show_chunk(player->location);
            break;
        case 'C':
            server_action_tile(SERVER_SHOW_CHUNK, player->location);
            break;
        case 'e':
        {
            InventoryElement * el = get_item_at_ppos(player);
            if (el)
                el->show(true);
            else
                printf("nothing to show\n");
            break;
        }
        case 'E':
            server_action_tile(SERVER_SHOW_ITEM, player->location);
            break;
        case 'h':
            help_question_mark();
            break;
        case 'i':
            player->inventory.show();
            break;
        case 'r':
            submenu = 0;
            break;
        case 'p':
            player->show();
            break;
        case 'P':
        {
            for (int p = 0; p < PLAYER_NUM; p++)
            {
                if (players[p])
                    players[p]->show();
            }
            break;
        }
        case 'o':
            objects.show(false);
            break;
    }
    return false;
}

void move_player(int dx, int dy)
{
    send_packet_move(client, dx, dy);
    /*InventoryElement * el = get_item_at_ppos(player);
    if (el)
        printf("\nthere is something here: %s\n", el->get_class_name());
*/
}
bool do_key_main(char k)
{
    if (k >= '0' && k <= '9')
    {
        active_hotbar = k - '1';
        if (active_hotbar < 0)
            active_hotbar = 9;
        InventoryElement * el = player->hotbar[active_hotbar];
        if (el)
        {
            char * des = player->get_el_description(el);
            printf("selected %d item: %s\n", active_hotbar, des ? des : "unknown");
        }
        else
            printf("selected %d slot\n", active_hotbar);
        return false;
    }
    switch (k)
    {
        case 'T':
        {
            server_action_tile(SERVER_TRACE_NETWORK, player->location);
            break;
        }
        case 't':
            trace_network += 1;
            break;
        case 'q':
            return true;
        case 'h':
            help();
            break;
        case 'n':
            use_network ^= true;
            break;
        case 'r':
            show_received ^= true;
            break;

        case 'i':
        {
            InventoryElement * el = get_item_at_ppos(player);
            if (el)
                send_packet_pickup(client, el->uid);
            else
                printf("nothing to pickup\n");
            break;
        }
        case '?':
            submenu = k;
            break;

        case 'I':
            put_element();
            break;
        case 'w':
            move_player(0, -1);
            break;
        case 's':
            move_player(0, 1);
            break;
        case 'a':
            move_player(-1, 0);
            break;
        case 'd':
            move_player(1, 0);
            break;
    }
    return false;
}

bool do_key(char k)
{
    switch (submenu)
    {
        default:
            return do_key_main(k);
        case '?':
            return do_key_question_mark(k);
    }
    return false;
}

bool check_chunk(int cx, int cy)
{
    if (cx < 0 || cy < 0 || cx >= WORLD_SIZE || cy >= WORLD_SIZE)
        return false;

    chunk * ch = world_table[cy][cx];
    if (!ch)
    {
        if (loaded_chunks[cy][cx] == CHUNK_NOT_LOADED)
        {
            send_packet_request_chunk(client, cx, cy);
            loaded_chunks[cy][cx] = CHUNK_LOADING;
            return false;
        }
        else
        {
            printf("waiting for chunk %d %d\n", cx, cy);
            return false;
        }
    }
    else
    {
        loaded_chunks[cy][cx] = CHUNK_LOADED;
    }
    return true;
}

void loop()
{
    print_status(0, "Welcome in PIME - TUI version for debug!");
    unsigned int total_recv = 0;

    while(!player->name.str[0])
    {
        printf("waiting for data\n");
        network_tick(client);
        sleep(1);
    }
    for (;;)
    {
        printf("\r%s%ld@[%d,%d][%d,%d] %c: ", player->get_name(), player->get_id(),
            player->location.chunk.map_x, player->location.chunk.map_y, player->location.chunk.x, player->location.chunk.y, submenu ? submenu : '#');

        char c = check_key();
        if (c)
        {
            if (do_key(c))
                break;
        }
        unsigned long start = get_time_usec();
        unsigned int recv = 0;

        if (use_network)
        {
            recv = network_tick(client);
        }
        total_recv += recv;

        unsigned long stop = get_time_usec();
        if (recv > max_recv)
            max_recv = recv;
        if (stop - start > max_time)
            max_time = stop - start;
        if (recv && show_received)
            printf("recv: %u/%u max=%u time=%lu us/%lu ms\n", recv, total_recv, max_recv, stop - start, max_time / 1000);

        for (int y=-1; y < 2; y++)
        {
            for (int x=-1; x < 2; x++)
            {
                check_chunk(player->location.chunk.map_x + x, player->location.chunk.map_y + y);
            }
        }
        long w = 20000 - (stop - start);
        if (w > 0 && !c)
            usleep(w);
    }
}

int main(int argc, char * argv[])
{
    const char * ip;

    if (argc < 2)
    {
        printf("usage: ./pime_tui <ip> [port]\n");
        printf("using localhost 127.0.0.1\n");
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
        loop();
    }
    tcsetattr(0, TCSANOW, &old_stdin);
}
