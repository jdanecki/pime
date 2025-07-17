
#include "main.h"
#include "window.h"
// Normal speed
#define UPDATE_DELAY 1000

// TURBO
// #define UPDATE_DELAY 0

SDL_Texture * map;
int auto_explore;

Player * players[PLAYER_NUM];
Player * player;

bool finish;

NetClient * client;

DHotbar hotbar;

extern void draw();

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

    // printf("Do you want music y/n? ");
    // a=getchar();
    // if (a=='y')
    // {
    //     printf("\nInitializing music\n");
    //     if (init_music())
    //         printf("Failed to initialize music!\n"); ;
    //     load_music();

    //     Mix_PlayChannel(0, music.music_one, 99999);
    //     Mix_PlayChannel(1, music.music_two, 99999);
    //     Mix_Volume(0, 0);
    //     Mix_Volume(1, 0);
    //     Mix_Pause(1);
    // } else printf("\nGame without music\n");
    // tcflush(0, TCIFLUSH);
    // tcsetattr(0, TCSANOW, &state);
}

int init_SDL()
{
    if (init_window())
        return 1;
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
            printf("missing directory with textures\n");
            return 2;
        }
    }

    // ret = stat("world", &statbuf);
    // if (ret)
    // {
    // #if defined(_WIN32)
    //     mkdir("world");
    // #else
    //     mkdir("world", 0777);
    // #endif
    // }

    load_textures();
    create_menus();
    map = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_STREAMING, WORLD_SIZE, WORLD_SIZE);

    return 0;
}

int setup()
{
    setbuf(stdout, nullptr); // fix for qtcreator console

    if (init_SDL() != 0)
        return 1;

    client = init();
    // FIXME
    current_npc = new Npc;

    return 0;
}

void do_auto_explore()
{
    /*if ((dst_map_x == player.map_x) && (dst_map_y == player.map_y)) {
        int dx = 5 - (rand() % 11);
        int dy = 5 - (rand() % 11);

       if (player.map_y+dy >= 0 && player.map_y+dy < WORLD_SIZE && player.map_x +dx >= 0 && player.map_x+dx < WORLD_SIZE)
       {
            if (!world_table[player.map_y+dy][player.map_x+dx]) {
                dst_map_x=player.map_x + dx;
                dst_map_y=player.map_y + dy;
            }
       }
    }
    if (dst_map_x > player.map_x) player.move(1, 0);
    if (dst_map_x < player.map_x) player.move(-1, 0);
    if (dst_map_y > player.map_y) player.move(0, 1);
    if (dst_map_y < player.map_y) player.move(0, -1);*/
}

void draw_dialogs()
{
    hotbar.draw(renderer);
}

void update_dialogs()
{
    hotbar.update();
}

void loop()
{
    // int dst_map_x=player.map_x;
    // int dst_map_y=player.map_y;

    print_status(0, "Welcome in game!");

    for (;;)
    {
        clear_window();

        if (handle_SDL_events())
            return;

        // TODO disconnect
        network_tick(client);
        update_dialogs();

        /*if (auto_explore) {
            do_auto_explore();
         } else {
                //dst_map_x=player.map_x;
                //dst_map_y=player.map_y;
         }*/
        draw();
        draw_dialogs();

        SDL_RenderPresent(renderer);
        if (!auto_explore)
            SDL_Delay(20);
    }
}

int main()
{
    if (setup())
        return 1;
    loop();
}
