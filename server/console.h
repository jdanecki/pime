#include <ncursesw/ncurses.h>
#include <string.h>
#include <stdlib.h>

#define RED "$COLOR_PAIR_1_"
#define GREEN "$COLOR_PAIR_2_"
#define ORANGE "$COLOR_PAIR_3_"
#define BLUE "$COLOR_PAIR_4_"
#define RESET "$COLOR_PAIR_0_"
#define MAX_OUTPUT_SIZE 189

extern int history_size;

extern WINDOW * in_w;
extern WINDOW * out_w;
extern WINDOW * status;
extern int win_w, win_h;

#define INPUT_SIZE 64
extern char in_buf[INPUT_SIZE + 1];
extern char ** out_buf;
extern int show_key;

extern int history_up;
extern int buf_pos;
extern bool scrolling;

void shift_output();
int add_to_output(const char * fmt, ...);
void clear_history();
void handle_resize();
char ncurses_init();
void ncurses_tick();
void print_status(int l, const char * format, ...);
