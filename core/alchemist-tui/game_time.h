#ifndef GAME_TIME_H
#define GAME_TIME_H

#include "../alchemist/el_list.h"
#include "alchemist/random_functions.h"

class Game_time
{
    int years;
    int days;
    int hours;
    int minutes;
    int seconds;
    char cur_time[30];

  public:
    void update_time(int sec);
    Game_time()
    {
        seconds = random_range(0, 59);
        minutes = random_range(0, 59);
        hours = random_range(0, 23);
        days = random_range(0, 359);
        years = random_range(1000, 2000);
    }
    char * get_time();
};

extern Game_time * game_time;
extern void (*callback_daily)();
#endif
