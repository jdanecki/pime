
#include "main.h"
#include "npc.h"
#include "window.h"
#include "../client-common/game.h"

SDL_Texture * map;

extern void draw();

void (*callback_daily)();
void daily_call()
{
}

void save(char with_player)
{
}
void load(char with_player)
{
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

bool setup(const char * ip, const char * port)
{
    setbuf(stdout, nullptr); // fix for qtcreator console

    if (init_SDL() != 0)
        return false;

    client = init(ip, port);
    if (client) return true;
    else return false;
}

int main(int argc, char * argv[])
{
    init_game("pime_SDL", argc, argv);
}
