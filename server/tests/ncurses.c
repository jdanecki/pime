#include <ncurses.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>

#define MAX_OUTPUT_SIZE 189
int history_size = 1024;
char ** out_buf;
int history_up;
int buf_pos;
bool scrolling=true;
#define RED "$COLOR_PAIR_1_"
#define GREEN "$COLOR_PAIR_2_"
#define ORANGE "$COLOR_PAIR_3_"
#define BLUE "$COLOR_PAIR_4_"
#define RESET "$COLOR_PAIR_0_"
WINDOW * win;
WINDOW * status;
int win_w, win_h;

void shift_output()
{
    for (int i = 1; i < history_size; i++)
    {
        strncpy(out_buf[i - 1], out_buf[i], MAX_OUTPUT_SIZE);
        out_buf[i][0]=0;
    }
}

int col=0;
int add_to_output(const char * fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    char buf[MAX_OUTPUT_SIZE+1];
    memset(buf, 0, MAX_OUTPUT_SIZE+1);
    vsnprintf(buf, MAX_OUTPUT_SIZE, fmt, args);
    va_end(args);

    int len=strlen(buf);
    if (col+strlen(buf) > MAX_OUTPUT_SIZE) {
       if (buf_pos < history_size-1) buf_pos++;
            else shift_output();
       col=0;
    }
    for (int ch=0; ch < len; ch++)
    {
        switch(buf[ch])
        {
            case 13: // \r
                col=0;
            break;
            case 10: // \n
                if (buf_pos < history_size-1) buf_pos++;
                else shift_output();
                col=0;
            break;
            default:
                out_buf[buf_pos][col]=buf[ch];
                if (col < MAX_OUTPUT_SIZE-1) col++;
            break;
        }
    }
    return 0;
}

void clear_history()
{
    for (int i = 0; i < history_size; i++)
    {
        memset(out_buf[i], 0, MAX_OUTPUT_SIZE);
    }
    history_up=0;
    buf_pos=0;
    add_to_output("cleared\n");
}

void ncurses_tick()
{
    werase(win);
    int lines=0;
    for (int i = 0; i < win_h; i++)
    {
        if (out_buf[i + history_up][0])
        {
            mvwaddnstr(win, i, 0, out_buf[i + history_up ], MAX_OUTPUT_SIZE);
            lines++;
        }
    }
    wrefresh(win);
}

void print_status(const char * format, ...)
{
    va_list args;
    char buf[128];
    va_start(args, format);
    vsnprintf(buf, 127, format, args);
    va_end(args);

    wprintw(status, "\r %s ", buf);
    wrefresh(status);
}

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
    win = newwin(h - 4, w, 0, 0);
    win_w=w;
    win_h=h-4;
    keypad(win,TRUE);

    init_pair(0, COLOR_WHITE, COLOR_BLACK);
    init_pair(1, COLOR_RED, COLOR_BLACK);
    init_pair(2, COLOR_GREEN, COLOR_BLACK);
    init_pair(3, COLOR_YELLOW, COLOR_BLACK);
    init_pair(4, COLOR_BLUE, COLOR_BLACK);
    init_pair(5,COLOR_BLACK,COLOR_BLUE);
    init_pair(6,COLOR_BLACK,COLOR_GREEN);

    status = newwin(1, w, h - 1, 0);
    wbkgd(status, COLOR_PAIR(5));

    wcolor_set(win, 0, NULL);
    scrollok(win, FALSE);

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
        key = wgetch(win);
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
                    history_up += 5;
                    break;
                case KEY_UP:
                    scrolling=false;
                    history_up -= 5;
                    if (history_up < 0)
                    history_up = 0;
                    break;
                case KEY_END:
                    scrolling=true;
                    break;
                }
            }
        }
        int x, y;
        getyx(win, y, x);
        print_status("key=%d buf_pos=%d col=%d history_up=%d history_size=%d w=%d h=%d x=%d y=%d",
                     key, buf_pos, col, history_up, history_size, win_w, win_h, x, y);
    }

    delwin(win);
    endwin();
    curs_set(1);
}
