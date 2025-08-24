#include "main.h"
#include <sys/ioctl.h>
#include <termios.h>
#include "../core/player.h"
#include "../SDL/player_actions.h"

Player * players[PLAYER_NUM];
Player * player;
extern int trace_network;
bool use_network = true;
int active_hotbar = 0;

bool finish;

NetClient * client;

struct termios old_stdin, stdin_tty;
void set_terminal()
{
    setbuf(stdout, nullptr);
    tcgetattr(0, &old_stdin);
    stdin_tty = old_stdin;
    stdin_tty.c_lflag &= ~(ECHO | ICANON | ECHOE | ISIG);
    stdin_tty.c_cc[VMIN] = 1;
    stdin_tty.c_cc[VTIME] = 0;
    tcsetattr(0, TCSANOW, &stdin_tty);

    tcflush(0, TCIFLUSH);
    ioctl(0, TCXONC, 1);
}

int setup(const char * ip, const char * port)
{
    set_terminal();

    client = init(ip, port);
    return 0;
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
};

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
    printf("n - use/don't use network\n");
    printf("i/I - pickup/drop item at player location\n");
    printf("q - quit\n");
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
        case 'c':
            if (world_table[player->map_y][player->map_x])
                world_table[player->map_y][player->map_x]->show();
            break;
        case 'C':
            server_action_tile(SERVER_SHOW_CHUNK, player->map_x, player->map_y, player->x, player->y);
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
            server_action_tile(SERVER_SHOW_ITEM, player->map_x, player->map_y, player->x, player->y);
            break;
        case 'h':
            help_question_mark();
            break;
        case 'i':
            player->inventory->show();
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
    }
    return false;
}

void move_player(int dx, int dy)
{
    send_packet_move(client, dx, dy);
    InventoryElement * el = get_item_at_ppos(player);
    if (el)
        printf("there is something here: %s", el->get_class_name());
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
            server_action_tile(SERVER_TRACE_NETWORK, player->map_x, player->map_y, player->x, player->y);
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

void loop()
{
    print_status(0, "Welcome in PIME - TUI version for debug!");

    for (;;)
    {
        printf("\r%c: ", submenu ? submenu : '#');
        char c = check_key();
        if (c)
        {
            if (do_key(c))
                break;
        }
        if (use_network)
            network_tick(client);
        usleep(20000);
    }
}

int main(int argc, char * argv[])
{
    if (argc < 2)
    {
        printf("usage: ./pime_SDL <ip> [port]\n");
        return 1;
    }
    const char * ip;
    ip = argv[1];
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
        return 1;
    loop();
    tcsetattr(0, TCSANOW, &old_stdin);
}
