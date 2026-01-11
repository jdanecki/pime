#include <stdlib.h>
#include <ctype.h>
#include <unistd.h>
#include "functions.h"

int main()
{
    int key=0;

	initscr();
	cbreak();
	noecho();
	nonl();
    int w, h;
	start_color();
    getmaxyx(stdscr, h, w);
//  lines, columns, y, x
    out_w = newwin(h - 1, w, 0, 0);
    win_w=w;
    win_h=h-1;
    keypad(out_w,TRUE);
    nodelay(out_w, TRUE);

    init_pair(0, COLOR_WHITE, COLOR_BLACK);
    init_pair(1, COLOR_RED, COLOR_BLACK);
    init_pair(2, COLOR_GREEN, COLOR_BLACK);
    init_pair(3, COLOR_YELLOW, COLOR_BLACK);
    init_pair(4, COLOR_BLUE, COLOR_BLACK);
    init_pair(5,COLOR_BLACK,COLOR_BLUE);
    init_pair(6,COLOR_BLACK,COLOR_GREEN);

    status = newwin(1, w, h - 1, 0);
    wbkgd(status, COLOR_PAIR(6));

    wcolor_set(out_w, 0, NULL);
    scrollok(out_w, FALSE);

    out_buf = (char **)calloc(history_size, sizeof(char *));
    for (int i = 0; i < history_size; i++)
    {
        out_buf[i] = (char *)calloc(MAX_OUTPUT_SIZE, sizeof(char *));
    }
    print_status("press a key\n");
    curs_set(0);
    while(key!= 'q')
    {
        ncurses_tick();
        key = wgetch(out_w);
        if (key!=ERR) {
            if (key < 127 )
            {
                switch(key)
                {
                    default: add_to_output("%c", key); break;
                    case 13:
                        col=0;
                        if (buf_pos < history_size-1)
                        {
                            buf_pos++;
                        }
                        else shift_output();
                        break;
                    case '1': add_to_output("line with enter\n"); break;
                    case '2': add_to_output("\nenter and line");break;
                    case '3': add_to_output("line with tab(\t)text"); break;
                    case '4': add_to_output("first line with enter\nsecond line with enter\n"); break;
                    case '5': add_to_output("first line with enter\rrewrite line\nsecond line with enter\n"); break;
                    case '6': add_to_output("1234567890"); break;
                    case '7':
                        for (int i=0; i< 10; i++) add_to_output("10 lines\n");
                    break;
                }
            }
            else {
                switch (key)
                {
                default: add_to_output(" key=%d\n", key); break;
                case KEY_HOME:
                    scrolling=false;
                    history_up = 0;
                    break;
                case KEY_DOWN:
                    scrolling=false;
                    if (history_up < (buf_pos-5)) history_up += 5;
                    break;
                case KEY_UP:
                    scrolling=false;
                    history_up -= 5;
                    if (history_up < 0) history_up = 0;
                    break;
                case KEY_END:
                    scrolling=true;
                    history_up=buf_pos-win_h+1;
                    if (history_up < 0) history_up=0;
                    break;
                }
            }
        }
        int x, y;
        getyx(out_w, y, x);
        if (key != ERR) {
            print_status("key=%d buf_pos=%d col=%d history_up=%d history_size=%d w=%d h=%d x=%d y=%d scrolling=%d",
                     key, buf_pos, col, history_up, history_size, win_w, win_h, x, y, scrolling);
        }
        usleep(20000);
    }

    delwin(out_w);
    endwin();
    curs_set(1);
}
