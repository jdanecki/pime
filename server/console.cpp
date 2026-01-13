#include "console.h"

int history_size = 1024;

WINDOW * in_w;
WINDOW * out_w;
WINDOW * status;
int win_w, win_h;

char in_buf[INPUT_SIZE + 1];
char ** out_buf;
int show_key = 0;

int history_up;
int buf_pos;
bool scrolling = true;

void print_status(int l, const char * format, ...)
{
    va_list args;
    char buf[128];
    va_start(args, format);
    vsnprintf(buf, 127, format, args);
    va_end(args);

    wprintw(status, "\r %s ", buf);
    wrefresh(status);
}

void shift_output()
{
    for (int i = 1; i < history_size; i++)
    {
        strncpy(out_buf[i - 1], out_buf[i], MAX_OUTPUT_SIZE);
        out_buf[i][0] = 0;
    }
}

int col = 0;
int CONSOLE_LOG(const char * fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    char buf[MAX_OUTPUT_SIZE + 1];
    memset(buf, 0, MAX_OUTPUT_SIZE + 1);
    vsnprintf(buf, MAX_OUTPUT_SIZE, fmt, args);
    va_end(args);

    int len = strlen(buf);
    if (col + strlen(buf) > MAX_OUTPUT_SIZE)
    {
        if (buf_pos < history_size - 1)
            buf_pos++;
        else
            shift_output();
        col = 0;
    }
    for (int ch = 0; ch < len; ch++)
    {
        switch (buf[ch])
        {
            case 13: // \r
                col = 0;
                break;
            case 10: // \n
                if (buf_pos < history_size - 1)
                    buf_pos++;
                else
                    shift_output();
                col = 0;
                break;
            default:
                out_buf[buf_pos][col] = buf[ch];
                if (col < MAX_OUTPUT_SIZE - 1)
                    col++;
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
    history_up = 0;
    buf_pos = 0;
    CONSOLE_LOG("cleared\n");
}

void ncurses_tick()
{
    werase(in_w);
    werase(out_w);
    box(in_w, 0, 0);
    wmove(in_w, 1, 2);
    char buf[128];
    snprintf(buf, 128, "> %s", in_buf);
    waddstr(in_w, buf);

    int lines = 0;
    int start_pos = history_up;
    if (scrolling)
    {
        start_pos = buf_pos - (win_h - 1);
        if (start_pos < 0)
            start_pos = 0;
        history_up = start_pos;
    }
    for (int i = 0; i < win_h; i++)
    {
        if (out_buf[start_pos + i][0])
        {
            mvwaddnstr(out_w, i, 0, out_buf[start_pos + i], MAX_OUTPUT_SIZE);
            lines++;
        }
    }
    if (scrolling)
    {
        if (lines >= win_h)
            history_up++;
    }

    wrefresh(out_w);
    wmove(in_w, 4 + strlen(in_buf), 1);
    wrefresh(in_w);
}

void handle_resize()
{
    int w, h;
    getmaxyx(stdscr, h, w);
    wresize(out_w, h - 4, w);
    wresize(in_w, 3, w);
    mvwin(in_w, h - 4, 0);
}

char ncurses_init()
{
    initscr();
    start_color();
    cbreak();
    noecho();
    keypad(stdscr, TRUE);
    curs_set(1);

    int w, h;
    getmaxyx(stdscr, h, w);
    out_w = newwin(h - 4, w, 0, 0);
    win_w = w;
    win_h = h - 4;

    in_w = newwin(3, w, h - 4, 0);
    status = newwin(1, w, h - 1, 0);

    wbkgd(status, COLOR_PAIR(5));
    wbkgd(in_w, COLOR_PAIR(6));
    //   keypad(out_w, TRUE);
    keypad(in_w, TRUE);
    //  nodelay(out_w, TRUE);
    nodelay(in_w, TRUE);

    out_buf = (char **)calloc(history_size, sizeof(char *));
    for (int i = 0; i < history_size; i++)
    {
        out_buf[i] = (char *)calloc(MAX_OUTPUT_SIZE, sizeof(char *));
    }

    memset(in_buf, 0, sizeof(char) * INPUT_SIZE);

    init_pair(0, COLOR_WHITE, COLOR_BLACK);
    init_pair(1, COLOR_RED, COLOR_BLACK);
    init_pair(2, COLOR_GREEN, COLOR_BLACK);
    init_pair(3, COLOR_YELLOW, COLOR_BLACK);
    init_pair(4, COLOR_BLUE, COLOR_BLACK);
    init_pair(5, COLOR_BLACK, COLOR_CYAN);
    init_pair(6, COLOR_BLACK, COLOR_GREEN);
    scrollok(out_w, FALSE);

    return 0;
}
