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
WINDOW * out_w;
WINDOW * status;
int win_w, win_h;

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
    werase(out_w);
    int lines=0;
    int start_pos=history_up;
    if (scrolling) {
        start_pos=buf_pos-(win_h-1);
        if (start_pos < 0) start_pos=0;
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
    if (scrolling) {
        if (lines >= win_h) history_up++;
    }
    
    wrefresh(out_w);
}
