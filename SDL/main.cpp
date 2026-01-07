#include <SDL2/SDL.h>
#include <cstdlib>
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <termios.h>
#include <unistd.h>

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

int main(int argc, char * argv[])
{
    start_game("pime_SDL", argc, argv);
}
