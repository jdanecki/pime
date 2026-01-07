#include <sys/ioctl.h>
#include <termios.h>
#include <time.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <termios.h>
#include <unistd.h>
#include "../core/time_core.h"
#include "../client-common/game.h"
#include "../client-common/text.h"
#include <raylib.h>

int main(int argc, char * argv[])
{
    start_game("pime_raylib", argc, argv);
}
