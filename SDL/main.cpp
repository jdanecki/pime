
#include "main.h"
#include "npc.h"
#include "window.h"
// Normal speed
#define UPDATE_DELAY 1000

// TURBO
// #define UPDATE_DELAY 0

SDL_Texture * map;
int auto_explore;

#define PlayerTYPE PlayerSDL
#include "../client-common/net.inl"

bool finish;

NetClient * client;

extern bool draw();

void (*callback_daily)();
void daily_call()
{
}

const char * direction_names[4] = {
    "up",
    "right",
    "down",
    "left",
};

void save(char with_player)
{
}
void load(char with_player)
{
}

int next_to(int x1, int y1, int x2, int y2)
{
    if (x1 == x2 && y1 == y2)
        return 2;

    if ((x1 == x2 + 1 || x1 == x2 - 1) && y1 == y2)
        return 1;
    if ((y1 == y2 + 1 || y1 == y2 - 1) && x1 == x2)
        return 1;
    return 0;
}

void intro()
{
    // int a;
    // struct termios state, new_state;
    // tcgetattr(0, &state);
    // new_state=state;
    // new_state.c_lflag &= ~(ECHO | ICANON |ECHOE| ISIG);
    // new_state.c_cc[VMIN] = 1;
    // tcsetattr(0, TCSANOW, &new_state);

    // CONSOLE_LOG("Do you want music y/n? ");
    //  a=getchar();
    //  if (a=='y')
    //  {
    //     CONSOLE_LOG("\nInitializing music\n");
    //      if (init_music())
    //         CONSOLE_LOG("Failed to initialize music!\n"); ;
    //      load_music();

    //     Mix_PlayChannel(0, music.music_one, 99999);
    //     Mix_PlayChannel(1, music.music_two, 99999);
    //     Mix_Volume(0, 0);
    //     Mix_Volume(1, 0);
    //     Mix_Pause(1);
    // } elseCONSOLE_LOG("\nGame without music\n");
    // tcflush(0, TCIFLUSH);
    // tcsetattr(0, TCSANOW, &state);
}

int init_SDL()
{
    int texture_size=32;
    if (init_window("pime", texture_size*CHUNK_SIZE + PANEL_WINDOW, texture_size*CHUNK_SIZE + STATUS_LINES))
    {
        return 1;
    }
    if (load_font())
        return 1;

    struct stat statbuf;
    int ret = stat("textures", &statbuf);
    if (ret)
    {
        chdir("..");
        ret = stat("textures", &statbuf);
        if (ret)
        {
            CONSOLE_LOG("missing directory with textures\n");
            return 2;
        }
    }

    load_textures();
    create_menus();
    map = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_ABGR8888, SDL_TEXTUREACCESS_STREAMING, WORLD_SIZE, WORLD_SIZE);

    return 0;
}

int setup(const char * ip, const char * port)
{
    setbuf(stdout, nullptr); // fix for qtcreator console

    if (init_SDL() != 0)
        return 1;

    client = init(ip, port);

    return 0;
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

    print_status(0, "Welcome in game!");

    for (;;)
    {
        clear_window();

        if (handle_SDL_events())
            return;

        // TODO disconnect
        network_tick(client);

        if (auto_explore)
        {
            do_auto_explore();
        }
        if (draw())
        {
            SDL_RenderPresent(renderer);
            if (!auto_explore)
                SDL_Delay(20);
        }
    }
}

int main(int argc, char * argv[])
{
    const char * ip;
    if (argc < 2)
    {
        CONSOLE_LOG("usage: ./pime_SDL <ip> [port]\n");
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
        return 1;
    loop();
}
